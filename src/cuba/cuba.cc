/**
 * @brief cuba.cc
 *
 * @date  : Aug 28, 2016
 * @author: lpzun
 */

#include "cuba.hh"

namespace cuba {

CUBA::CUBA(const string& filename, const string& initl, const string& final) :
		mapping_Q(), active_Q(), active_R(), PDS(), initl_c(), final_c() {
	parse_input_pda(filename);
	initl_c = parse_input_cfg(initl);
	final_c = parse_input_cfg(final);
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
void CUBA::parse_input_pda(const string& filename) {
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
	pda_state s1, s2; /// control states
	pda_alpha l1, l2;  /// stack symbols
	string sep;
	vertex src = 0, dst = 0;
	while (new_in >> s1 >> l1 >> sep >> s2 >> l2) {
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

//	for (int i = 0; i < mapping_Q.size(); ++i) {
//		for (int j = 0; j < mapping_Q[i].size(); ++j) {
//			cout << mapping_Q[i][j] << " ";
//		}
//		cout << "\n";
//	}
//	cout << "\n";

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
thread_config CUBA::parse_input_cfg(const string& s) {
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
	return pds_parser::create_thread_config_from_str(s);
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
	auto fsa = create_reachability_automaton();
	cout << "reachability automaton: \n";
	cout << fsa << endl;
	if (prop::OPT_REACHABILITY) {
		cout << final_c << " ";
		if (is_recongnizable(fsa, final_c))
			cout << "is reachable";
		else
			cout << "is unreachable";
		cout << "\n";
	}

}

/**
 * The procedure of building the reachability automaton FSA for a PDA
 * @return a finite automaton
 */
finite_automaton CUBA::create_reachability_automaton() {
	cout << "Initial configuration: " << initl_c << endl;
	auto fsa = create_init_fsa(initl_c);
	cout << "initial automaton: \n";
	cout << fsa << endl;
	return post_kleene(fsa);
}

/**
 * build the reachability automation for the initial configuration of a PDA
 * @param c: an initial configuration
 * @return a finite automaton
 */
finite_automaton CUBA::create_init_fsa(const thread_config& c) {
	fsa_state state_pda = thread_state::S;
	fsa_alpha alpha_pda = thread_state::L;

	auto w = c.get_stack();
	auto q = c.get_state(); /// the final state

	/// the state of fsa
	auto state_fsa = state_pda + w.size();
	/// the transitions
	fsa_delta delta;
	auto s = state_pda; /// s is an assitant state
	while (!w.empty()) {
		delta[q].emplace(q, s, w.top());
		w.pop();
		q = s, ++s; /// update the final state
	}

	return finite_automaton(state_fsa, alpha_pda, delta, state_pda, q);
}

/**
 * build the reachability automaton for a finite automaton
 * @param A
 * @return a finite automaton
 */
finite_automaton CUBA::post_kleene(const finite_automaton& A) {
	auto state = A.get_states();
	auto alpha = A.get_alphabet();
	auto delta = A.get_transitions();

	unordered_map<int, fsa_state> map_r_to_aux_state;
	for (auto i = 0; i < active_R.size(); ++i) {
		if (active_R[i].get_oper_type() == type_stack_operation::PUSH)
			map_r_to_aux_state.emplace(i, state++);
	}

	deque<fsa_transition> worklist;
	fsa_delta explored;
	fsa_delta reversed; /// used to deal with ...

	/// initialize the worklist
	for (const auto& p : delta)
		worklist.insert(worklist.begin(), p.second.begin(), p.second.end());

	while (!worklist.empty()) {
		const auto t = worklist.front(); /// FSA transition (p, a, q)
		worklist.pop_front();

		/// FSA transition (p, a, q)
		const auto& p = t.get_src();
		const auto& a = t.get_label();
		const auto& q = t.get_dst();

		const auto& ret = explored[p].insert(t);
		if (!ret.second || p >= thread_state::S)
			continue;

		if (a != alphabet::EPSILON) { /// if label != epsilon
			const auto& pda_transs = PDS[retrieve(p, a)];
			for (const auto& rid : pda_transs) {
				/// (p, a) -> (p', a')
				const auto& r = active_R[rid]; /// PDA transition
				const auto& _p = active_Q[r.get_dst()].get_state();  /// state
				const auto& _a = active_Q[r.get_dst()].get_symbol(); /// label

				switch (r.get_oper_type()) {
				case type_stack_operation::POP: { /// pop operation
					worklist.emplace_back(_p, q, alphabet::EPSILON);
				}
					break;
				case type_stack_operation::OVERWRITE: { /// overwrite operation
					worklist.emplace_back(_p, q, _a);
				}
					break;
				default: { /// push operation
					const auto& q_new = map_r_to_aux_state[rid];
					worklist.emplace_back(_p, q_new, _a);
					explored[q_new].emplace(q_new, q, a);

					const auto& fsa_transs = reversed[q_new];
					for (const auto& _t : fsa_transs) {
						if (_t.get_label() == alphabet::EPSILON)
							worklist.emplace_back(_t.get_src(), q_new, a);
					}
					reversed[q_new].emplace(_p, q_new, _a);
				}
					break;
				}
			}
		} else { /// if label == epsilon
			const auto& fsa_transs = explored[q];
			for (const auto& _t : fsa_transs)  /// _t = (q, a', q')
				worklist.emplace_back(p, _t.get_dst(), _t.get_label());
		}
	}
	return finite_automaton(state, alpha, explored, A.get_initials(),
			A.get_accept_state());
}

/**
 * This procedure is to retrieve the id of thread state
 * @param s: control state
 * @param l: stack symbol
 * @return thread state id
 */
vertex CUBA::retrieve(const pda_state& s, const pda_alpha& l) {
	return mapping_Q[s][l];
}

/**
 * To determine whether c is reachable in the PDA
 * @param fsa : a reachability automaton
 * @param c   : a configuration
 * @return bool
 *         true : if c is reachable
 *         false: otherwise.
 */
bool CUBA::is_recongnizable(const finite_automaton& fsa,
		const thread_config& c) {
	if (c.get_state() >= fsa.get_initials())
		throw cuba_runtime_error("c's control state is illegal!");

	queue<pair<fsa_state, int>> worklist;
	worklist.emplace(c.get_state(), 0);

	const auto& w = c.get_stack();
	while (!worklist.empty()) {
		const auto u = worklist.front();
		worklist.pop();

		const auto& state = u.first;
		const auto& depth = u.second;

		auto ifind = fsa.get_transitions().find(state);
		if (ifind == fsa.get_transitions().end())
			continue;

		for (const auto& r : ifind->second)
			if (r.get_label() == w.get_worklist()[depth]) {
				if (depth + 1 == w.size()) {
					if (r.get_dst() == fsa.get_accept_state())
						return true;
				} else {
					worklist.emplace(r.get_dst(), depth + 1);
				}
			}
	}
	return false;
}

/**
 * Determine the equivalence of two finite automatons. This implementation
 * is based on the Hopcroft-Karp algorithm.
 * @param fsa1
 * @param fsa2
 * @return bool
 */
bool CUBA::is_equivalent(const finite_automaton& fsa1,
		const finite_automaton& fsa2) {
	return true;
}

/**
 * This is the main procedure to do the context-bounded analysis. It implements the
 * algorithm in QR'05 paper.
 * @param n   : the number of threads
 * @param k   : the upper bound of context switches
 * @param g_in: the initial controcl state
 * @param A_in
 * @return
 */
deque<aggregate_config> CUBA::context_bounded_analysis(const size_t& n,
		const size_k& k, const pda_state& g_in, const finite_automaton& A_in) {
	/// Step 1: declare the data structures used in this procedure:
	///
	/// 1.1 <reached> : the set of reachable aggregate configurations
	///
	/// Initialization: empty set
	deque<aggregate_config> reached;

	/// 1.2 <worklist>: the set of aggregate configurations are waiting
	/// for handling; note its elements are pairs that each of them
	/// contains an aggregate configuration and the necessary context
	/// switches to reach it.
	///
	/// Initialization: {(<g_in, (A_in, ..., A_in)>, 0)}
	queue<pair<aggregate_config, size_k>> worklist;
	aggregate_config cfg_in(g_in, n, A_in);
	worklist.emplace(cfg_in, 0);

	/// Step 2: operate on the elements, aka, pairs in the worklist one by one.
	/// This is like a BFS procedure.
	while (!worklist.empty()) {
		/// 2.1 remove a aggregate configuration from worklist
		auto p = worklist.front();
		worklist.pop();
		/// 2.2 check whether its context switches already reaches to the upper
		/// bound; if so, discard the current element.
		if (p.second < k)
			continue;
		/// 2.3 extract the aggregate configuration from the pair.
		const auto& cfg = p.first;
		const auto& automatons = cfg.get_automatons();
		for (int i = 0; i < automatons.size(); ++i) {
			const auto& _A = post_kleene(automatons[i]);
			for (const auto& _g : project_G(_A)) {
				const auto& _cfg = compose(_g, automatons, i);
				worklist.emplace(_cfg, p.second + 1);
				reached.push_back(_cfg);
			}
		}
	}
	return reached;
}

/**
 * This procedure projects all states g such that {g| exist w. <g, w> in L(A)}.
 *
 * @param A
 * @return a list of states
 */
deque<fsa_state> CUBA::project_G(const finite_automaton& A) {
	deque<fsa_state> states;
	unordered_map<fsa_state, deque<fsa_state>> transpose;
	for (const auto& p : A.get_transitions()) {
		for (const auto& r : p.second) {
			transpose[r.get_dst()].push_back(p.first);
		}
	}
	return BFS_visit(A.get_accept_state(), transpose, A.get_initials());
}

/**
 * This is to compute all states which has a path to the accept state
 * @param root: the accept state
 * @param adj : adjacency list of automaton
 * @param initials: the set of initial states
 * @return the set of initial states which can reach the accept state
 */
deque<fsa_state> CUBA::BFS_visit(const fsa_state& root,
		const unordered_map<fsa_state, deque<fsa_state>>& adj,
		const fsa_state_set& initials) {
	deque<fsa_state> G;

	/// Mark whether a state is already visited or not:
	/// to avoid a cycle
	unordered_set<fsa_state> explored;
	explored.emplace(root);

	/// the worklist
	queue<fsa_state> worklist;
	worklist.push(root);
	while (!worklist.empty()) {
		const auto u = worklist.front();
		worklist.pop();

		if (u < initials)
			G.emplace_back(u);

		auto ifind = adj.find(u);
		if (ifind != adj.end()) {
			for (const auto& v : ifind->second) {
				/// if u is already visited, then skip it
				if (explored.find(u) != explored.end())
					continue;
				explored.emplace(v);
				worklist.push(v);
			}
		}
	}
	return G;
}

/**
 * This procedure is to composite a aggregate configuration.
 * @param _g
 * @param automatons
 * @param idx
 * @return aggregate configuration
 */
aggregate_config CUBA::compose(const pda_state& _g,
		const vector<finite_automaton>& automatons, const int& idx) {
	vector<finite_automaton> W;
	W.reserve(automatons.size());
	for (int i = 0; i < automatons.size(); ++i) {
		if (i == idx)
			W.push_back(this->anonymize(automatons[i], _g));
		else
			W.push_back(this->rename(automatons[i], _g));
	}
	return aggregate_config(_g, W);
}

/**
 * This is the rename procedure: it rename the state state of A, if any,
 * to _g.
 * @param A
 * @param _g
 * @return a finite automaton
 */
finite_automaton CUBA::rename(const finite_automaton& A, const pda_state& _g) {
	finite_automaton _A(A);
	_A.set_start_state(_g);
	return _A;
}

/**
 *
 * This is the anonymize procedure: it rename all states except _g to
 * fresh states delete all states except _g
 * @param fsa
 * @param _g
 * @param is_rename
 * @return a finite automaton
 */
finite_automaton CUBA::anonymize(const finite_automaton& fsa,
		const pda_state& _g, const bool& is_rename) {
	if (is_rename)
		return anonymize_by_rename(fsa, _g);
	return anonymize_by_delete(fsa, _g);
}

/**
 * This is the anonymize procedure: it rename all states except _g to
 * fresh states.
 * @param A
 * @param _g
 * @return a finite automaton
 */
finite_automaton CUBA::anonymize_by_delete(const finite_automaton& A,
		const pda_state& _g) {
	auto transs = A.get_transitions();
	for (auto g = 0; g < A.get_initials(); ++g) {
		if (g == _g)
			continue;
		transs.erase(g);
	}
	return finite_automaton(A.get_states(), A.get_alphabet(), transs,
			A.get_initials(), A.get_accept_state());
}

/**
 * This is the anonymize procedure: it rename all states except _g to
 * fresh states.
 * @param A
 * @param _g
 * @return a finite automaton
 */
finite_automaton CUBA::anonymize_by_rename(const finite_automaton& A,
		const pda_state& _g) {
	auto states = A.get_states();
	auto transs = A.get_transitions();
	for (auto g = 0; g < A.get_initials(); ++g) {
		if (g == _g)
			continue;
		transs.emplace(states++, transs[g]);
	}
	return finite_automaton(states, A.get_alphabet(), transs, A.get_initials(),
			A.get_accept_state());
}

} /* namespace cuba */
