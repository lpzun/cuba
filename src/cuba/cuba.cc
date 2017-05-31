/**
 * @brief cuba.cc
 *
 * @date  : Aug 28, 2016
 * @author: lpzun
 */

#include "cuba.hh"

namespace cuba {

/**
 * constructor of CUBA
 * @param filename
 * @param initl
 * @param final
 */
CUBA::CUBA(const string& initl, const string& final, const string& filename) :
		initl_c(0, 1), final_c(0, 1), CPDA() {
	initl_c = parser::parse_input_cfg(initl);
	final_c = parser::parse_input_cfg(final);
	CPDA = parser::parse_input_cpds(filename);
}

/**
 * destructor
 */
CUBA::~CUBA() {
}

/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the definitions for context-unbounded
/// analysis.
/////////////////////////////////////////////////////////////////////////

/**
 * The procedure of context-bounded analysis
 * @param n: the number of threads
 * @param k: the number of context switches
 */
void CUBA::context_bounded_analysis(const size_k& k, const size_t& n) {
	vector<store_automaton> W;
	for (auto i = 0; i < CPDA.size(); ++i) {
		auto fsa = create_init_automaton(i);
		cout << "pushdown store automaton " << i << "\n";
		cout << fsa << endl;
		W.push_back(fsa);
	}
	symbolic_config cfg_I(initl_c.get_state(), W);
	auto R = context_bounded_analysis(k, cfg_I);
}

/**
 * The procedure of building the reachability automaton FSA for a PDA
 * @return a finite automaton
 */
store_automaton CUBA::create_store_automaton(const size_t i) {
	cout << "Initial configuration: " << initl_c << "\n";
	auto fsa = create_init_automaton(i);
	cout << "Initial automaton: \n";
	cout << fsa << endl;
	return post_kleene(fsa, CPDA[i]);
}

/**
 *
 * build the reachability automation for an initial configuration of a PDA
 * @param c: an initial configuration
 * @param PDA
 * @return
 */
store_automaton CUBA::create_init_automaton(const size_t i) {
	/// step 0: set up the accept state
	const auto& P = CPDA[i];
	const auto& q_I = initl_c.get_state();
	auto w = initl_c.get_stacks()[i];
	auto q_F = create_accept_state(CPDA[i].get_states());
	/// step 1: set up the set of states
	fsa_state_set states { q_F };

	/// step 2: set up the set of alphas
	auto alphas = P.get_alphas();

	/// step 3: set up the set of initial states
	fsa_state_set initials { q_I };

	/// step 4: set up the set of transitions
	fsa_delta delta;
	auto q = q_I;
	auto _q = create_interm_state(states);
	while (!w.empty()) {
		const auto a = w.top();
		w.pop();
		if (w.empty())
			_q = q_F;

		states.emplace(_q); /// add the intermediate state
		delta[q].emplace(q, _q, a); /// add a new transition

		q = _q++; /// set up new source & destination states
	}
	return store_automaton(states, alphas, delta, initials, q_F);
}

/**
 *
 * @param states
 * @return
 */
fsa_state CUBA::create_accept_state(const fsa_state_set& states) {
	if (states.size() == 0)
		throw cuba_runtime_error("no control state!");
	auto s = *(states.rbegin()); /// s is an intermediate state
	return ++s;
}
/**
 *
 * @param states
 * @return
 */
fsa_state CUBA::create_interm_state(const fsa_state_set& states) {
	if (states.size() == 0)
		throw cuba_runtime_error("no control state!");
	auto s = *(states.rbegin()); /// s is an intermediate state
	return ++s;
}

/**
 * build the reachability automaton for a finite automaton
 * @param A
 * @return a finite automaton
 */
store_automaton CUBA::post_kleene(const store_automaton& A,
		const pushdown_automaton& P) {
	auto states = A.get_states();
	auto alphas = A.get_alphas();
	fsa_delta deltas; /// the transitions of Post*(A)

	unordered_map<int, fsa_state> map_r_to_aux_state;
	auto s = create_interm_state(states);
	for (auto i = 0; i < P.get_actions().size(); ++i) {
		if (P.get_actions()[i].get_oper_type() == type_stack_operation::PUSH)
			map_r_to_aux_state.emplace(i, s++);
	}

	deque<fsa_transition> worklist;
	fsa_delta epsilon_R; /// used to deal with ...

	/// initialize the worklist
	for (const auto& p : A.get_transitions()) {
		worklist.insert(worklist.begin(), p.second.begin(), p.second.end());
	}

	while (!worklist.empty()) {
		const auto t = worklist.front(); /// FSA transition (p, a, q)
		worklist.pop_front();

		/// FSA transition (p, a, q)
		const auto& p = t.get_src();
		const auto& a = t.get_label();
		const auto& q = t.get_dst();

		const auto& ret = deltas[p].insert(t);
		if (!ret.second || p >= thread_state::S)
			continue;

		if (a != alphabet::EPSILON) { /// if label != epsilon
			auto ifind = P.get_pda().find(thread_state(p, a));
			if (ifind != P.get_pda().end()) {
				for (const auto& rid : ifind->second) {

					const auto& r = P.get_actions()[rid]; /// PDA transition
					const auto& _p = r.get_dst().get_state(); /// state
					auto _W = r.get_dst().get_stack(); /// stack: maintain a copy

					switch (r.get_oper_type()) {
					case type_stack_operation::POP: { /// pop
						worklist.emplace_back(_p, q, alphabet::EPSILON);
						epsilon_R[_p].emplace(_p, q, alphabet::EPSILON); /// epsilon transitions
					}
						break;
					case type_stack_operation::OVERWRITE: { /// overwrite
						worklist.emplace_back(_p, q, _W.top());
					}
						break;
					default: { /// push
						const auto& q_in = map_r_to_aux_state[rid];
						const auto& a1 = _W.top();
						_W.pop();
						const auto& a2 = _W.top();
						_W.pop();

						worklist.emplace_back(_p, q_in, a1);
						deltas[q_in].emplace(q_in, q, a2);

						for (const auto& _t : epsilon_R[q_in]) {
							if (_t.get_label() == alphabet::EPSILON)
								worklist.emplace_back(_t.get_src(), q_in, a2);
						}
					}
						break;
					}
				}
			}
		} else { /// if label == epsilon
			for (const auto& _t : deltas[q])  /// _t = (q, a', q')
				worklist.emplace_back(p, _t.get_dst(), _t.get_label()); /// (p, a', q')
		}
	}

	return store_automaton(states, alphas, deltas, P.get_states(),
			A.get_accept());
}

/**
 * To determine whether c is reachable in the PDA
 * @param A: a reachability automaton
 * @param c: a configuration
 * @return bool
 *         true : if c is reachable
 *         false: otherwise.
 */
bool CUBA::is_recongnizable(const store_automaton& A, const thread_config& c) {

	queue<pair<fsa_state, int>> worklist;
	worklist.emplace(c.get_state(), 0);

	const auto& w = c.get_stack();
	while (!worklist.empty()) {
		const auto u = worklist.front();
		worklist.pop();

		const auto& state = u.first;
		const auto& depth = u.second;

		auto ifind = A.get_transitions().find(state);
		if (ifind == A.get_transitions().end())
			continue;

		for (const auto& r : ifind->second)
			if (r.get_label() == w.get_worklist()[depth]) {
				if (depth + 1 == w.size()) {
					if (r.get_dst() == A.get_accept())
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
 * @param A1
 * @param A2
 * @return bool
 */
bool CUBA::is_equivalent(const store_automaton& A1, const store_automaton& A2) {
	return true;
}

/// a pair of configuration used in QR algorithm
using tau = pair<symbolic_config, size_k>;
/**
 * This is the main procedure to do the context-bounded analysis. It implements the
 * algorithm in QR'05 paper.
 * @param k   : the upper bound of context switches
 * @param cfg_I: the initial symbolic configuration
 * @return a set of reachable symbolic configuration
 */
deque<symbolic_config> CUBA::context_bounded_analysis(const size_k k,
		const symbolic_config& cfg_I) {
	/// Step 1: declare the data structures used in this procedure:
	///
	/// 1.1 <explored>: the set of reachable aggregate configurations
	/// Initialization: empty set
	deque<symbolic_config> global_R;

	/// 1.2 <worklist>: the set of aggregate configurations are waiting
	/// for handling; note its elements are pairs that each of them
	/// contains an aggregate configuration and the necessary context
	/// switches to reach it.
	///
	/// Initialization: {(<q_I, (A1_I, ..., An_I)>, 0)}
	queue<tau, deque<tau>> worklist;
	worklist.emplace(cfg_I, 0);

	/// 1.3 <topped_R>: the set of reachable tops of configurations.
	/// We obtain this by computing the symbolic configurations.
	vector<deque<config_top>> topped_R(thread_state::S);

	/// Step 2: operate on the elements, aka, pairs in the worklist
	/// one by one. This is like a BFS procedure.
	while (!worklist.empty()) {
		/// 2.1 remove a aggregate configuration from worklist
		auto p = worklist.front();
		worklist.pop();

		/// 2.2 check whether its context switches already reaches
		/// to the upper bound; if so, discard the current element.
		if (p.second >= k)
			continue;

		/// 2.3 extract the aggregate configuration from the pair.
		const auto& automata = p.first.get_automatons();
		for (int i = 0; i < automata.size(); ++i) {
			const auto& _A = post_kleene(automata[i], CPDA[i]);
			for (const auto& _q : project_G(_A)) {
				const auto& _cfg = compose(_q, automata, i);
				worklist.emplace(_cfg, p.second + 1);
				global_R.push_back(_cfg);
			}
		}
	}
	return global_R;
}

/**
 * This procedure projects all states g such that {g| exist w. <g, w> in L(A)}.
 *
 * @param A
 * @return a list of states
 */
deque<fsa_state> CUBA::project_G(const store_automaton& A) {
	deque<fsa_state> states;
	unordered_map<fsa_state, deque<fsa_state>> transpose;
	for (const auto& p : A.get_transitions()) {
		for (const auto& r : p.second) {
			transpose[r.get_dst()].push_back(p.first);
		}
	}
	return BFS_visit(A.get_accept(), transpose, A.get_initials());
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

		if (initials.find(u) != initials.end())
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
 * This procedure is to composite a symbolic configuration.
 * Please keep in mind that a symbolic configuration represents
 * a set of concrete configurations.
 * @param q_I: an initial state
 * @param automata: a list of automata
 * @param idx
 * @return symbolic configuration
 */
symbolic_config CUBA::compose(const pda_state& q_I,
		const vector<store_automaton>& automata, const int& idx) {
	vector<store_automaton> W;
	W.reserve(automata.size());
	for (int i = 0; i < automata.size(); ++i) {
		if (i == idx)
			W.push_back(this->anonymize(automata[i], q_I));
		else
			W.push_back(this->rename(automata[i], q_I));
	}
	return symbolic_config(q_I, W);
}

/**
 * This is the rename procedure: it rename the state state of A, if any,
 * to q_I.
 * @param A
 * @param q
 * @return a store automaton
 */
store_automaton CUBA::rename(const store_automaton& A, const pda_state& q_I) {
	return store_automaton(A.get_states(), A.get_alphas(), A.get_transitions(),
			{ q_I }, A.get_accept());
}

/**
 *
 * This is the anonymize procedure: it rename all states except q to
 * fresh states delete all states except q
 * @param A
 * @param q_I
 * @param is_rename
 * @return a store automaton
 */
store_automaton CUBA::anonymize(const store_automaton& A, const pda_state& q_I,
		const bool& is_rename) {
	if (is_rename)
		return anonymize_by_rename(A, q_I);
	return anonymize_by_split(A, q_I);
}

/**
 * This is the anonymize procedure: it deletes all states except q_I to
 * fresh states.
 *
 * The procedure is an onion-peel approach:
 *   1. we remove the all related states
 *
 * @param A
 * @param q_I
 * @return a store automaton
 */
store_automaton CUBA::anonymize_by_split(const store_automaton& A,
		const pda_state& q_I) {
	fsa_state_set states;
	fsa_delta deltas;

	queue<fsa_state> worklist;
	worklist.emplace(q_I);
	while (!worklist.empty()) {
		const auto q = worklist.front();
		worklist.pop();

		/// insert q into the states of anonymized automaton
		auto ret = states.emplace(q);
		if (!ret.second)
			continue; /// skip q if q has been processed

		/// find q's successors, if applicable
		auto ifind = A.get_transitions().find(q);
		if (ifind == A.get_transitions().end())
			continue; /// skip q if q has no successor

		/// processed q and its successors
		for (const auto& r : ifind->second) {
			deltas[q].emplace(r);
			worklist.emplace(r.get_dst());
		}
	}

	return store_automaton(states, A.get_alphas(), deltas, { q_I },
			A.get_accept());
}

/**
 * This is the anonymize procedure: it rename all states except q
 * to fresh states.
 * @param A
 * @param q_I
 * @return a finite automaton
 */
store_automaton CUBA::anonymize_by_rename(const store_automaton& A,
		const pda_state& q_I) {
	auto states = A.get_states();
	auto transs = A.get_transitions();
	for (auto q : A.get_initials()) {
		if (q == q_I)
			continue;
		//transs.emplace(states++, transs[g]);
	}
	return store_automaton(states, A.get_alphas(), transs, A.get_initials(),
			A.get_accept());
}

/////////////////////////////////////////////////////////////////////////
/// Extract all tops of configurations from a symbolic configuration tau
///
/////////////////////////////////////////////////////////////////////////
///
/**
 * Extract all configuration tops of symbolic configuration tau
 * @param tau
 * @return a set of configuration tops
 */
vector<config_top> CUBA::extract_config_tops(const symbolic_config& tau) {
	const auto q = tau.get_state();
	vector<set<pda_alpha>> toppings(tau.get_automatons().size());
	for (auto i = 0; i < tau.get_automatons().size(); ++i) {
		toppings[i] = extract_top_symbols(tau.get_automatons()[i], q);
	}
	const auto& worklist = cross_product(toppings);
	vector<config_top> tops;
	tops.reserve(worklist.size());
	for (const auto& W : worklist) {
		tops.emplace_back(q, W);
	}
	return tops;
}

/**
 * Extract all top symbols of a store automaton
 * @param A
 * @param q
 * @return a set of top symbols
 */
set<pda_alpha> CUBA::extract_top_symbols(const store_automaton& A,
		const pda_state q) {
	set<pda_alpha> tops; /// the set of top symbols
	queue<pda_state> worklist; ///
	worklist.emplace(q);
	while (!worklist.empty()) {
		const auto p = worklist.front();
		worklist.pop();

		auto ifind = A.get_transitions().find(p);
		if (ifind == A.get_transitions().end()) {
			continue;
		}
		/// iterate all out-going edges
		for (const auto& r : ifind->second) {
			if (r.get_label() == alphabet::EPSILON) /// epsilon label
				worklist.push(r.get_dst());
			else
				tops.insert(r.get_label());
		}
	}
	return tops;
}

/**
 * Compute the cross product of all tops
 * @param tops
 * @return the cross product of tops
 */
vector<vector<pda_alpha>> CUBA::cross_product(
		const vector<set<pda_alpha>>& tops) {
	vector<vector<pda_alpha>> worklist;
	worklist.emplace_back(vector<pda_alpha>());
	for (const auto& ts : tops) {
		vector<vector<pda_alpha>> templist;
		for (const auto s : ts) {
			for (auto v : worklist) {
				v.emplace_back(s);
				templist.emplace_back(v);
			}
		}
		worklist.swap(templist);
	}
	return worklist;
}

} /* namespace cuba */
