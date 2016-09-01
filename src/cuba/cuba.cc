/**
 * @brief cuba.cc
 *
 * @date  : Aug 28, 2016
 * @author: lpzun
 */

#include "cuba.hh"

namespace cuba {

CUBA::CUBA(const string& filename, const string& initl, const string& final) :
		mapping_Q(), active_Q(), active_R(), PDS(), initl_TS(), final_TS() {
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
			mapping_Q.emplace(src_TS, state_id);
			src = state_id++;
			visited[s1][l1] = true;
		} else {
			auto ifind = mapping_Q.find(src_TS);
			if (ifind != mapping_Q.end()) {
				src = ifind->second;
			} else
				throw cuba_runtime_error("thread state is mistakenly marked!");
		}

		/// step 2: handle (s2, l2)
		const thread_state dst_TS(s2, l2);
		if (!visited[s2][l2]) {
			active_Q.emplace_back(dst_TS);
			mapping_Q.emplace(dst_TS, state_id);
			dst = state_id++;
			visited[s2][l2] = true;
		} else {
			auto ifind = mapping_Q.find(dst_TS);
			if (ifind != mapping_Q.end())
				dst = ifind->second;
			else
				throw cuba_runtime_error("thread state is mistakenly marked!");
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

void CUBA::context_unbounded_analysis(const size_t& n) {
	auto size = reachable_thread_states(n);
	cout << "The number of reachable thread states is: " << size << endl;
}

/**
 * To compute the set of reachable thread states
 * @param n
 * @return the number of reachable thread states
 */
uint CUBA::reachable_thread_states(const size_t& n) {
	cstack W(n, sstack());
	for (auto i = 0; i < n; ++i) {
		W[i].push(initl_TS.get_symbol());
	}

	antichain worklist;
	worklist.emplace_back(initl_TS.get_state(), W);

	vector<antichain> R(thread_state::S);

	marking_Q = vector<vector<bool>>(thread_state::S,
			vector<bool>(thread_state::L, false));
	marking(initl_TS.get_state(), initl_TS.get_symbol());

	while (!worklist.empty()) {
		const auto tau = worklist.front();
		worklist.pop_front();
		cout << tau << "\n";

		if (is_reachable(tau, R[tau.get_state()]))
			continue;

		const auto& images = step(tau);
		for (const auto& _tau : images) {
			cout << "  " << _tau << "\n";
			worklist.emplace_back(_tau);
		}
		/// add current configuration into reachable set
		R[tau.get_state()].emplace_back(tau);
	}

	int counting = 0;
	for (int i = 0; i < thread_state::S; ++i) {
		for (int j = 0; j < thread_state::L; ++j) {
			if (marking_Q[i][j])
				++counting;
		}
	}
	return counting;
}

/**
 *
 * @param tau
 * @param R
 * @return bool
 */
bool CUBA::is_reachable(const global_config& tau, const antichain& R) {
	for (const auto& g : R) {
		if (g == tau)
			return true;
	}
	return false;
}

/**
 * step forward, to compute the direct successors
 * @param tau
 * @return a list of global configurations
 */
antichain CUBA::step(const global_config& tau) {
	antichain worklist;
	const auto& q = tau.get_state();
	const auto& W = tau.get_stacks();
	for (auto i = 0; i < W.size(); ++i) {

		marking(q, W[i].top());

		auto ifind = mapping_Q.find(thread_state(q, W[i].top()));
		if (ifind != mapping_Q.end()) {
			const auto& transs = PDS[ifind->second];
			for (const auto& rid : transs) {
				const auto& r = active_R[rid];
				const auto& dst = active_Q[r.get_dst()];

				marking(dst.get_state(), dst.get_symbol());

				switch (r.get_oper_type()) {
				case type_stack_operation::PUSH: {
					auto _W = W;
					_W[i].push(dst.get_symbol());
					worklist.emplace_back(dst.get_state(), _W);
				}
					break;
				case type_stack_operation::POP: {
					auto _W = W;
					_W[i].pop();
					worklist.emplace_back(dst.get_state(), _W);
				}
					break;
				default: {
					auto _W = W;
					_W[i].overwrite(dst.get_symbol());
					worklist.emplace_back(dst.get_state(), _W);
				}
					break;
				}
			}
		}
	}
	return worklist;
}

/**
 *
 * @param s
 * @param l
 */
void CUBA::marking(const control_state& s, const stack_symbol& l) {
	if (!marking_Q[s][l])
		marking_Q[s][l] = true;
}

} /* namespace cuba */
