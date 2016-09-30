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
	auto fsa = compute_fsa();
	cout << "reachability automaton: \n";
	cout << fsa << endl;
}

finite_automaton CUBA::compute_fsa() {
	sstack w;
	w.push(initl_TS.get_symbol());
	w.push(initl_TS.get_symbol());
	thread_config c(initl_TS.get_state(), w);
	auto fsa = compute_init_fsa(c);
	cout << "initial automaton: \n";
	cout << fsa << endl;
	return compute_post_fsa(fsa);
}

/**
 * build the reachability automation for the initial configuration of a PDA
 * @param c: an initial configuration
 * @return a finite automaton
 */
finite_automaton CUBA::compute_init_fsa(const thread_config& c) {
	cout << c << endl;
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

	return finite_automaton(state_fsa, alpha_pda, delta, q);
}

/**
 * build the reachability automaton for a finite automaton
 * @param A
 * @return
 */
finite_automaton CUBA::compute_post_fsa(const finite_automaton& A) {
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
//		cout << t << ":\n";

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
				const auto& _p = active_Q[r.get_dst()].get_state(); /// state
				const auto& _a = active_Q[r.get_dst()].get_symbol(); /// label
//				cout << " " << active_Q[r.get_src()] << r.get_oper_type()
//						<< "->" << active_Q[r.get_dst()] << "\n";

				switch (r.get_oper_type()) {
				case type_stack_operation::POP: { /// pop operation
					worklist.emplace_back(_p, q, alphabet::EPSILON);
//					cout << " " << worklist.back() << "\n";
				}
					break;
				case type_stack_operation::OVERWRITE: { /// overwrite operation
					worklist.emplace_back(_p, q, _a);
//					cout << " " << worklist.back() << "\n";
				}
					break;
				default: { /// push operation
					const auto& q_new = map_r_to_aux_state[rid];
					worklist.emplace_back(_p, q_new, _a);
//					cout << " " << worklist.back() << "\n";
					explored[q_new].emplace(q_new, q, a);
//					cout << " (" << q_new << "," << q << "," << a << ")"
//							<< "\n";

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
	return finite_automaton(state, alpha, explored, A.get_accept_state());
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

} /* namespace cuba */
