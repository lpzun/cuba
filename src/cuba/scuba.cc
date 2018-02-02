/**
 * cuba.cc
 *
 * @date  : Aug 28, 2016
 * @author: Peizun Liu
 */

#include "cuba.hh"

namespace cuba {
/**
 * constructor
 * @param initl: initial global state
 * @param final: final global state
 * @param filename: input CPDs
 * @param n: the number of threads, for parameterized system only
 */
symbolic_cuba::symbolic_cuba(const string& initl, const string& final,
		const string& filename) :
		base_cuba(initl, final, filename) {
}

/**
 * destructor
 */
symbolic_cuba::~symbolic_cuba() {
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
void symbolic_cuba::context_unbounded_analysis(const size_k k_bound) {
	/// step 1: set up the initial configurations
	vector<store_automaton> W;
	for (uint i = 0; i < CPDA.size(); ++i) {
		W.emplace_back(create_init_automaton(i));
	}
	symbolic_state cfg_I(initl_c.get_state(), W);

	/// step 2: perform context-unbounded analysis
	cout << "\n" << "Initial configuration: " << cfg_I << endl;
	auto is_reachable = context_bounded_analysis(k_bound, cfg_I);
	if (prop::OPT_PROB_REACHABILITY && is_reachable) {
		cout << final_c << " is reachable!" << endl;
	}
}

/// a pair of configuration used in QR algorithm
/**
 * This is the main procedure to do the context-bounded analysis. It implements
 * the algorithm in QR'05 paper.
 * @param k  : the upper bound of context switches
 * @param c_I: the initial symbolic configuration
 * @return a set of reachable symbolic configuration
 */
bool symbolic_cuba::context_bounded_analysis(const size_k k_bound,
		const symbolic_state& c_I) {
	/// Step 1: declare the data structures used in this procedure:
	///
	/// 1.1 <currLevel> = S_{k} \ S_{k-1}: the set of symbolic configurations
	/// which are reached in the kth context for processing. It's initialized
	/// as { <q_I|(A1_I, ..., An_I)> }.
	deque<symbolic_state> currLevel;
	currLevel.emplace_back(c_I);
	/// 1.2 <k>: contexts, k = 0 represents the initial configuration c_I
	size_k k = 0;
	/// 1.3 <global_R>: the set of reachable global configurations.
	/// We obtain this by computing the symbolic configurations.
	vector<deque<symbolic_state>> global_R;
	global_R.reserve(k_bound + 1);
	global_R.emplace_back(deque<symbolic_state> { c_I });
	/// 1.4 <top_R>: the set of reachable tops of configurations.
	/// We obtain this by computing the symbolic configurations.
	vector<set<visible_state>> top_R(thread_visible_state::S);
	/// Compute top_R_0
	converge(global_R, k, top_R);
	/// Step 2: compute all reachable configurations with up to k_bound
	/// contexts. If k_bound = 0, the procedure will either loops until R/top_R
	/// collapses or forever.
	while (k_bound == 0 || k < k_bound) {
		/// <nextLevel> = S_{k+1} \ S_{k}: the set of symbolic configurations
		/// reached in the (k+1)st context. It's initialized as empty.
		deque<symbolic_state> nextLevel;
		/// step 2.1 compute nextLevel, or S_{k+1}: iterate over all elements
		/// in the currLevel. This is a BFS-like procedure.
		while (!currLevel.empty()) {
			/// 2.1.1 remove a aggregate configuration from currLevel.
			const auto c = currLevel.front();
			currLevel.pop_front();

			/// 2.1.2 extract the aggregate configuration from the pair.
			const auto& automata = c.get_automata();
			for (uint i = 0; i < automata.size(); ++i) {
				if (automata[i].empty())
					continue;
				const auto& _A = post_kleene(automata[i], CPDA[i]);
				for (const auto& _q : project_Q(_A))
					nextLevel.push_back(compose(_q, _A, automata, i));
			}
		}

		/// step 2.2 if all elements in currLevel has been processed, then move
		/// onto the (k + 1)st context.
		currLevel.swap(nextLevel), ++k;
		/// step 2.3 store R_{k+1}: the set of global configurations
		global_R.emplace_back(currLevel); /// k + 1 now
		/// step 2.4 convergence detection for top_R
		if (converge(global_R, k, top_R)) {
			cout << "=> sequence top_R collapses at " << (k == 0 ? k : k - 1)
					<< "\n";
			cout << "======================================" << endl;
			return true;
		}
	}
	cout << "======================================" << endl;
	return false;
}

/**
 * The procedure of building the reachability automaton FSA for a PDA
 * @return a finite automaton
 */
store_automaton symbolic_cuba::create_store_automaton(const size_n i) {
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
store_automaton symbolic_cuba::create_init_automaton(const size_n i) {
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
fsa_state symbolic_cuba::create_accept_state(const fsa_state_set& states) {
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
fsa_state symbolic_cuba::create_interm_state(const fsa_state_set& states) {
	if (states.size() == 0)
		throw cuba_runtime_error("no control state!");
	auto s = *(states.rbegin()); /// s is an intermediate state
	return ++s;
}

/**
 * Post*(A): all reachable states from states represented by A
 * @param A
 * @return a finite automaton
 */
store_automaton symbolic_cuba::post_kleene(const store_automaton& A,
		const pushdown_automaton& P) {
	auto states = A.get_states();
	auto alphas = A.get_alphas();
	fsa_delta deltas; /// the transitions of Post*(A)

	unordered_map<int, fsa_state> map_r_to_aux_state;
	auto s = create_interm_state(states);
	for (uint i = 0; i < P.get_actions().size(); ++i) {
		if (P.get_actions()[i].get_oper_type() == type_stack_operation::PUSH)
			map_r_to_aux_state.emplace(i, s++);
	}

	deque<fsa_transition> worklist;
	fsa_delta epsilon_R; /// used to handle epsilon transitions

	/// initialize the worklist
	for (const auto& trans : A.get_transitions()) {
		for (const auto& t : trans.second) {
			worklist.emplace_back(t);
			if (t.get_label() == alphabet::EPSILON)
				epsilon_R[t.get_src()].emplace(t);
		}
	}

	while (!worklist.empty()) {
		const auto t = worklist.front(); /// FSA transition (p, a, q)
		worklist.pop_front();

		/// FSA transition (p, a, q)
		const auto& p = t.get_src();
		const auto& a = t.get_label();
		const auto& q = t.get_dst();

		const auto& ret = deltas[p].insert(t);
		if (!ret.second || p >= thread_visible_state::S)
			continue;

		if (a != alphabet::EPSILON) { /// if label != epsilon
			auto ifind = P.get_program().find(thread_visible_state(p, a));
			if (ifind != P.get_program().end()) {
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
bool symbolic_cuba::is_recongnizable(const store_automaton& A,
		const thread_state& c) {
	queue<pair<fsa_state, uint>> worklist;
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
bool symbolic_cuba::is_equivalent(const store_automaton& A1,
		const store_automaton& A2) {
	return true;
}

/**
 * This procedure projects all states g such that {g| exist w. <g, w> in L(A)}.
 *
 * @param A
 * @return a list of states
 */
set<fsa_state> symbolic_cuba::project_Q(const store_automaton& A) {
	/// A precise but also inefficient way to extract all initial states
	/// I comment it away
	/**
	 unordered_map<fsa_state, deque<fsa_state>> transpose;
	 for (const auto& p : A.get_transitions()) {
	 for (const auto& r : p.second) {
	 transpose[r.get_dst()].push_back(p.first);
	 }
	 }
	 return BFS_visit(A.get_accept(), transpose, A.get_initials());
	 */
	return A.get_start();
}

/**
 * This is to compute all states which has a path to the accept state
 * @param root: the accept state
 * @param adj : adjacency list of automaton
 * @param initials: the set of initial states
 * @return the set of initial states which can reach the accept state
 */
set<fsa_state> symbolic_cuba::BFS_visit(const fsa_state& root,
		const unordered_map<fsa_state, deque<fsa_state>>& adj,
		const fsa_state_set& initials) {
	set<fsa_state> Q;

	/// Mark whether a state is already visited or not:
	/// to avoid a cycle
	unordered_set<fsa_state> explored;

	/// the worklist
	queue<fsa_state> worklist;
	worklist.push(root);
	while (!worklist.empty()) {
		const auto u = worklist.front();
		worklist.pop();

		/// if u is already visited, then skip it
		auto ret = explored.emplace(u);
		if (!ret.second)
			continue;

		/// store u if u is an initial state
		if (initials.find(u) != initials.end())
			Q.emplace(u);

		auto ifind = adj.find(u);
		if (ifind == adj.end())
			continue;

		for (const auto v : ifind->second)
			worklist.emplace(v);
		explored.emplace(u);
	}
	return Q;
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
symbolic_state symbolic_cuba::compose(const pda_state& q_I,
		const store_automaton& Ai, const vector<store_automaton>& automata,
		const size_n& i) {
	vector<store_automaton> W;
	W.reserve(automata.size());
	for (uint j = 0; j < automata.size(); ++j) {
		if (j == i)
			W.push_back(this->anonymize(Ai, q_I));
		else
			W.push_back(this->rename(automata[j], q_I));
	}
	return symbolic_state(q_I, W);
}

/**
 * This is the rename procedure: it rename the state state of A, if any,
 * to q_I.
 * @param A
 * @param q
 * @return a store automaton
 */
store_automaton symbolic_cuba::rename(const store_automaton& A,
		const pda_state& q_I) {
	if (A.get_start().size() == 0)
		throw cuba_runtime_error("rename: no initial state");
	auto old = *A.get_start().begin();
	auto ifind = A.get_transitions().find(old);
	if (ifind == A.get_transitions().end())
		return store_automaton(A.get_states(), A.get_alphas(),
				A.get_transitions(), { q_I }, A.get_accept());
	auto deltas = A.get_transitions();
	deltas.erase(old);
	for (const auto& r : ifind->second) {
		deltas[q_I].emplace(q_I, r.get_dst(), r.get_label());
	}
	return store_automaton(A.get_states(), A.get_alphas(), deltas, { q_I },
			A.get_accept());
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
store_automaton symbolic_cuba::anonymize(const store_automaton& A,
		const pda_state& q_I, const bool& is_rename) {
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
store_automaton symbolic_cuba::anonymize_by_split(const store_automaton& A,
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
	states.erase(q_I); /// remove the initial state q_I from states,
					   /// as states only have intermediate states
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
store_automaton symbolic_cuba::anonymize_by_rename(const store_automaton& A,
		const pda_state& q_I) {
	auto states = A.get_states();
	auto transs = A.get_transitions();
	for (auto q : A.get_start()) {
		if (q == q_I)
			continue;
		//transs.emplace(states++, transs[g]);
	}
	return store_automaton(states, A.get_alphas(), transs, A.get_start(),
			A.get_accept());
}

/////////////////////////////////////////////////////////////////////////
/// Extract all tops of configurations from a symbolic configuration tau
///
/////////////////////////////////////////////////////////////////////////

/**
 * Determine whether top configuration converges or not
 * @param R
 * @param k
 * @param top_R
 * @return
 */
bool symbolic_cuba::converge(const vector<deque<symbolic_state>>& R,
		const size_k k, vector<set<visible_state>>& top_R) {
	cout << "======================================\n";
	cout << "context " << k << "\n";
	const auto cnt_new_top_cfg = top_mapping(R[k], top_R);
	if (cnt_new_top_cfg == 0) {
		if (is_convergent())
			return true;
		cout << "=> sequence top_R plateaus at " << k << "\n";
	}
	return false;
}

/**
 * Determine if OS3 converges or not
 * @return bool
 *         true : if OS3 converges
 *         false: otherwise
 */
bool symbolic_cuba::is_convergent() {
	for (const auto& s : generators) {
		if (!s.empty())
			return false;
	}
	return true;
}

/**
 *
 * @param R
 * @param topped_R
 * @return
 */
uint symbolic_cuba::top_mapping(const deque<symbolic_state>& R,
		vector<set<visible_state>>& topped_R) {
	uint cnt_new_top_cfg = 0;
	for (const auto& c : R) {
		for (const auto& top_c : top_mapping(c)) {
			auto ret = topped_R[top_c.get_state()].emplace(top_c);
			if (ret.second) {
				cout << string(2, ' ') << top_c << "\n";
				++cnt_new_top_cfg;
				/// updating approx_X
				auto ifind = generators[top_c.get_state()].find(top_c);
				if (ifind != generators[top_c.get_state()].end()) {
					generators[top_c.get_state()].erase(ifind);
				}
			}
		}
	}
	return cnt_new_top_cfg;
}

/**
 * Extract all configuration tops of symbolic configuration tau
 * @param tau
 * @return a set of configuration tops
 */
vector<visible_state> symbolic_cuba::top_mapping(const symbolic_state& tau) {
	const auto q = tau.get_state();
	vector<set<pda_alpha>> toppings(tau.get_automata().size());

	for (uint i = 0; i < tau.get_automata().size(); ++i) {
		toppings[i] = top_mapping(tau.get_automata()[i], q);
		//for (const auto& s : toppings[i])
		//	cout << i << "===================" << s << endl;
	}

	const auto& worklist = cross_product(toppings);

	vector<visible_state> tops;
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
set<pda_alpha> symbolic_cuba::top_mapping(const store_automaton& A,
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
				worklist.emplace(r.get_dst());
			tops.emplace(r.get_label());
		}
	}
	return tops;
}

/**
 * Compute the cross product of all tops
 * @param tops
 * @return the cross product of tops
 */
vector<vector<pda_alpha>> symbolic_cuba::cross_product(
		const vector<set<pda_alpha>>& tops) {
	vector<vector<pda_alpha>> worklist;
	worklist.emplace_back(vector<pda_alpha>());
	for (const auto& ts : tops) {
		vector<vector<pda_alpha>> temp;
		for (const auto s : ts) {
			for (auto W : worklist) {
				W.emplace_back(s);
				temp.emplace_back(W);
			}
		}
		worklist.swap(temp);
	}
	return worklist;
}

} /* namespace cuba */
