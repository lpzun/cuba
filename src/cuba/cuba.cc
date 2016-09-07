/**
 * @brief cuba.cc
 *
 * @date  : Aug 28, 2016
 * @author: lpzun
 */

#include "cuba.hh"

namespace cuba {

CUBA::CUBA(const string& filename, const string& initl, const string& final) :
		mapping_Q(), active_Q(), active_R(), PDS(), initl_TS(), final_TS(), //
		k_bound(1) {
	parse_PDS(filename);
	initl_TS = parse_TS(initl);
	final_TS = parse_TS(final);
}

CUBA::~CUBA() {
}

/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the definitions for parsing input PDS
///
/////////////////////////////////////////////////////////////////////////

/**
 * To parse the input PDS
 * @param filename
 */
void CUBA::parse_PDS(const string& filename) {
	if (filename == "X")
		throw cuba_runtime_error("Please assign the input file!");

	/// original input file, possibly with comments
	ifstream org_in(filename.c_str());
	if (!org_in.good())
		throw cuba_runtime_error("Input file does not exist!");
	pds_parser::remove_comments(org_in, "/tmp/tmp.pds.no_comment",
			prop::COMMENT);
	org_in.close();

	ifstream new_in("/tmp/tmp.pds.no_comment");
	new_in >> thread_state::S >> thread_state::L;

	active_Q.reserve(thread_state::S * thread_state::L);
	active_R.reserve(thread_state::S * thread_state::L);

	vertex state_id = 0; /// define the unique transition id
	edge trans_id = 0;   /// define the unique thread state id;

	/// to mark if a thread state is visited or not
	vector<vector<bool>> visited(thread_state::S,
			vector<bool>(thread_state::L, false));

	mapping_Q = vector<vector<vertex>>(thread_state::S,
			vector<vertex>(thread_state::L, 0));

	control_state s1, s2; /// control states
	stack_symbol l1, l2;  /// stack symbols
	string sep;
	vertex src = 0, dst = 0;
	while (new_in >> s1 >> l1 >> sep >> s2 >> l2) {
//		cout << s1 << " " << l1 << " " << sep << " " << s2 << " " << l2 << "\n";

		/// step 1: handle (s1, l1)
		const thread_state src_TS(s1, l1);
		if (!visited[s1][l1]) {
			active_Q.emplace_back(src_TS);
			mapping_Q[s1][l1] = state_id;
			src = state_id++;
			visited[s1][l1] = true;
		} else {
			src = mapping_Q[s1][l1];
		}

		/// step 2: handle (s2, l2)
		const thread_state dst_TS(s2, l2);
		if (!visited[s2][l2]) {
			active_Q.emplace_back(dst_TS);
			mapping_Q[s2][l2] = state_id;
			dst = state_id++;
			visited[s2][l2] = true;
		} else {
			dst = mapping_Q[s2][l2];
		}

		/// step 3: handle transitions
		if (sep.length() != 3) {
			throw cuba_runtime_error("the type of transiton is wrong!");
		}

		active_R.emplace_back(src, dst,
				pds_parser::parse_type_stack_operation(sep[0]),
				pds_parser::parse_type_synchronization(sep[1]));
		PDS[src].emplace_back(trans_id++);
	}
	new_in.close();
	active_Q.shrink_to_fit();

	if (prop::OPT_PRINT_ADJ || prop::OPT_PRINT_ALL) {
		cout << "The original PDS: " << "\n";
		cout << thread_state::S << " " << thread_state::L << "\n";
		for (const auto& r : active_R) {
			cout << active_Q[r.get_src()] << " ";
			cout << r.get_oper_type();
			cout << r.get_sync_type();
			cout << ">";
			cout << " " << active_Q[r.get_dst()];
			cout << "\n";
		}
		cout << endl;
	}
}

/**
 * To parse the input initial/final thread state
 * @param s
 * @return thread state
 */
thread_state CUBA::parse_TS(const string& s) {
	if (s.find('|') == std::string::npos) { /// s is store in a file
		ifstream in(s.c_str());
		if (in.good()) {
			string content;
			std::getline(in, content);
			in.close();
			return pds_parser::create_thread_state_from_str(content);
		} else {
			throw cuba_runtime_error(
					"parse_input_SS: input state file is unknown!");
		}
	}
	return pds_parser::create_thread_state_from_str(s);
}

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following are the definitions for context-unbounded
/// analysis.
/////////////////////////////////////////////////////////////////////////

/**
 * The procedure of context-bounded analysis
 * @param n: the number of threads
 * @param k: the number of context switches
 */
void CUBA::context_bounded_analysis(const size_t& n, const size_k& k) {
	auto size = bounded_reachability(n, k);
	cout << "The number of reachable thread states is: " << size << endl;
}

/**
 * This procedure is to compute the set of reachable thread/global states
 * of a program P running by n threads and with up to k context switches.
 * @param n: the number of threads
 * @param k: the upper bound of context switches
 * @return the number of reachable thread/global states
 */
uint CUBA::bounded_reachability(const size_t& n, const size_k& k) {
	/// step 1: build the initial  global configuration
	///         Here we build the set of initial stacks
	stack_vec W_0(n, sstack()); /// the set of initial stacks
	for (auto tid = 0; tid < n; ++tid) {
		W_0[tid].push(initl_TS.get_symbol());
	}

	/// step 2: set the data structures that used in the exploration
	///         procedure
	/// the set of unexplored global configurations
	antichain worklist;
	/// set the initial global configuration
	worklist.emplace_back(0, 0, initl_TS.get_state(), W_0);
	/// the set of reachable global configurations
	vector<antichain> explored(thread_state::S);

	/// step 2.1: this part is used to mark the reachable thread states
	this->reachable_T = vector<vector<bool>>(thread_state::S,
			vector<bool>(thread_state::L, false));
	this->marking(initl_TS.get_state(), initl_TS.get_symbol());

	/// step 3: the exploration procedure: it is based on BFS
	while (!worklist.empty()) {
		/// step 3.1: remove an element from worklist
		const auto tau = worklist.front();
		worklist.pop_front();
		cout << tau << "\n"; /// deleting ---------------------

		/// step 3.2: discard it if tau is already explored
		if (is_reachable(tau, explored[tau.get_state()]))
			continue;

		/// step 3.3: compute its successors and process them
		///           one by one
		const auto& images = step(tau);
		for (const auto& _tau : images) {
			cout << "  " << _tau << "\n"; /// deleting ---------------------
			worklist.emplace_back(_tau);
		}
		/// step 3.4: add current configuration to explored set
		explored[tau.get_state()].emplace_back(tau);
	}

	/// step 4: compute the set of reachable thread states
	int counting = 0;
	for (auto s = 0; s < thread_state::S; ++s) {
		for (auto l = 0; l < thread_state::L; ++l) {
			if (reachable_T[s][l])
				++counting;
		}
	}
	return counting; /// return the number of reachable thread states
}

/**
 * This procedure is to determine whether there exists g \in explored such that
 *  (1) g == tau;
 *  (2) g.id == tau.id /\ g.s == tau.s /\ g.W == tau.W /\ g.k < tau.k.
 * It returns true if any of above conditions satisfies. Otherwise, it returns
 * false.
 *
 * One special case is that: there exists g \in explored such that
 *    g.id == tau.id /\ g.s == tau.s /\ g.W == tau.W /\ g.k > tau.k,
 * then, the procedure replaces g by tau except returning false.
 *
 * @param tau
 * @param R
 * @return bool
 */
bool CUBA::is_reachable(const global_config& tau, antichain& R) {
	for (auto& g : R) {
		if (g == tau) {
			if (g.get_context_k() > tau.get_context_k()) {
				g.set_context_k(tau.get_context_k());
				return false;
			}
			return true;
		}
	}
	return false;
}

/**
 * step forward, to compute the direct successors of tau
 * @param tau: a global configuration
 * @return a list of successors, aka. global configurations
 */
antichain CUBA::step(const global_config& tau) {
	antichain successors;
	const auto& q = tau.get_state();     /// the control state of tau
	const auto& W = tau.get_stacks();    /// the stacks of tau
	const auto& k = tau.get_context_k(); /// the context switches to tau
	const auto& t = tau.get_thread_id(); /// the previous thread id to tau

	/// step 1: if context switches already reach to the uppper bound
	if (k == k_bound)
		return successors;

	/// step 2: iterate over all threads
	for (auto tid = 0; tid < W.size(); ++tid) {
		/// step 2.1: set context switches
		///           if context switch occurs, then k = k + 1;
		auto _k = tid == t ? k : k + 1;
		/// step 2.2: iterator over all successor of current thread state
		const auto& transs = PDS[retrieve(q, W[tid].top())];
		for (const auto& rid : transs) { /// rid: transition id
			const auto& r = active_R[rid];
			const auto& dst = active_Q[r.get_dst()];
			auto _W = W; /// duplicate the stacks in current global conf.
			switch (r.get_oper_type()) {
			case type_stack_operation::PUSH: { /// push operation
				_W[tid].push(dst.get_symbol());
				successors.emplace_back(tid, _k, dst.get_state(), _W);
			}
				break;
			case type_stack_operation::POP: { /// pop operation
				if (_W[tid].pop())
					successors.emplace_back(tid, _k, dst.get_state(), _W);
			}
				break;
			default: { /// overwrite operation
				if (_W[tid].overwrite(dst.get_symbol()))
					successors.emplace_back(tid, _k, dst.get_state(), _W);
			}
				break;
			}
			/// marking thread state dst is reachable
			this->marking(dst.get_state(), dst.get_symbol());
		}

	}
	return successors; /// the set of successors of tau
}

/**
 * This procedure is to retrieve the id of thread state
 * @param s: control state
 * @param l: stack symbol
 * @return thread state id
 */
vertex CUBA::retrieve(const control_state& s, const stack_symbol& l) {
	return mapping_Q[s][l];
}

/**
 * set thread state (s,l) as reachable
 * @param s
 * @param l
 */
void CUBA::marking(const control_state& s, const stack_symbol& l) {
	if (!reachable_T[s][l])
		reachable_T[s][l] = true;
}

} /* namespace cuba */
