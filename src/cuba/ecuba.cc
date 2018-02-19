/**
 * @brief explore.cc
 *
 * @date  : Sep 28, 2016
 * @author: lpzun
 */

#include "cuba.hh"

namespace cuba {
/**
 * Constructor for explicit version of CUBA; calling base_cuba
 *   Set up the initial global state, target state, concurrent pushdown
 *   system and the overapproximation of reachable top states.
 *
 * @param initl initial global state
 * @param final final global state
 * @param filename input CPDs
 */
explicit_cuba::explicit_cuba(const string& initl, const string& final,
		const string& filename) :
		base_cuba(initl, final, filename) {
}

/**
 * destructor
 */
explicit_cuba::~explicit_cuba() {

}

/**
 * The procedure of context-bounded analysis
 * @param k: the number of context switches
 */
void explicit_cuba::context_unbounded_analysis(const size_k k_bound) {
	bool cycle = false;
	size_n tid = 0;
	while (tid < CPDA.size()) {
		if (finite_context_reachability(tid)) {
			cycle = true;
			break;
		}
		++tid;
	}
	if (cycle) {
		cout << "Finite-context reachability is unsatisfiable...\n";
		return;
	}
	const auto convergent = k_bounded_reachability(k_bound, initl_c);
	if (prop::OPT_PROB_REACHABILITY) {
		if (reachable)
			cout << "=> " << final_c << " is reachable!" << endl;
		else if (convergent)
			cout << "=> " << final_c << " is unreachable!" << endl;
	}
}

/**
 * This procedure is to compute the set of reachable thread/global states
 * of a concurrent program P with up to k contexts. It returns true if
 * observation sequence converges, false otherwise.
 *
 * @param k_bound: the upper bound of contexts
 * @param c_I    : the initial global states
 * @return bool
 */
bool explicit_cuba::k_bounded_reachability(const size_k k_bound,
		const explicit_state& c_I) {
	/// step 1: set the data structures that used in the explicit exploration
	///         procedure
	/// 1.1 <currLevel> = R_{k} \ R_{k-1}: the set of unexplored global conf-
	/// igurations which are reached in the kth context for processing. It's
	/// initialized as { <q_I | (w1_I, ..., wn_I> }
	antichain currLevel;
	currLevel.emplace_back(CPDA.size(), c_I.get_state(), c_I.get_stacks());
	/// 1.2 <k>: contexts, k = 0 represented the initial states c_I
	size_k k = 0;
	/// 1.3 <global_R>: the sequences of reachable global states.
	vector<vector<antichain>> global_R;
	global_R.reserve(k_bound + 1);
	global_R.emplace_back(vector<antichain>(thread_visible_state::S));
	global_R[k][c_I.get_state()].emplace_back(CPDA.size(), c_I.get_state(),
			c_I.get_stacks());

	/// 1.4 <top_R>: the sequences of visible states. We obtained the
	/// sequence from R directly.
	vector<set<visible_state>> top_R(thread_visible_state::S);
	/// step 2: compute all reachable states with up to k_bound contexts.
	while (k_bound == 0 || k <= k_bound) {
		/// step 2.0 <nextLevel> = R_{k+1} \ R_{k}: the set of explicit
		/// states reached in the (k+1)st context. It's initialized
		/// as empty.
		antichain nextLevel;
		/// step 2.1: compute nextLevel, or R_{k+1}: iterate over all elements
		/// in the currLevel. This is a BFS-like procedure.
		while (!currLevel.empty()) {
			/// 2.1.1: remove an element from currLevel
			const auto c = currLevel.front();
			currLevel.pop_front();
			/// 2.1.3: compute its successors and process them one by one
			/// The successors without context switch
			const auto& k_images = step(c, false);
			for (const auto& _c : k_images) {
				/// 2.1.4: skip _c if it's already been reached
				if (!update_R(global_R, k, _c))
					continue;
				/// 2.1.5: add _c to worklist if it's unreachable
				currLevel.emplace_back(_c);
			}
			/// The successors with context switch
			const auto& k_plus_images = step(c, true);
			for (const auto& _c : k_plus_images) {
				/// 2.1.4: skip _c if it's already been reached
				if (!update_R(global_R, k + 1, _c))
					continue;
				/// 2.1.5: add _c to worklist if it's unreachable
				nextLevel.emplace_back(_c);
			}
		}
		/// step 2.2: convergence detection
		/// 2.2.1: OS1 collapses
		if (nextLevel.size() == 0) {
			cout << "=> sequence R and T(R) collapses at "
					<< (k == 0 ? k : k - 1) << "\n";
			return true;
		}

		if (converge(global_R[k], k, top_R)) {
			cout << "=> sequence T(R) collapses at " << (k == 0 ? k : k - 1)
					<< "\n";
			return true;
		}
		/// if reachability and the target visible state is reachable
		if (prop::OPT_PROB_REACHABILITY && reachable) {
			break;
		}
		/// step 2.3: if all states in currLevel have been processed,
		/// then move onto the (k+1)st contexts.
		currLevel.swap(nextLevel), ++k;
	}

	return false;
}

/**
 * Step forward, to compute the direct successors of tau. It returns a list
 * of successors, aka. global states
 * @param tau a global state
 * @param is_switch
 * @return
 */
antichain explicit_cuba::step(const explicit_state_tid& tau,
		const bool is_switch) {
	antichain successors;
	/// step 1: if context switches already reach to the upper bound
	const auto& q = tau.get_state(); /// the control state of tau
	const auto& W = tau.get_stacks(); /// the stacks of tau
	/// step 2:
	if (!is_switch) {
		/// step 2.1: Obtain the successors without any context switch
		step(q, W, tau.get_thread_id(), successors);
	} else {
		/// step 2.2: Obtain the successors with any context switch. So, the
		/// algorithm iterates over all threads but the one producing tau
		for (uint tid = 0; tid < W.size(); ++tid) {
			/// step 2.2.1: deal with the situation of the thread whose index
			/// ID = tid is "dead".
			/// @TODO WARNING: this means we don't allow an action whose source
			/// is empty. Need to revisit
			if (tid == tau.get_thread_id())
				continue;
			/// step 2.2.2: add successors obtained by performing thread tid
			step(q, W, tid, successors);
		}
	}
	return successors; /// the set of successors of tau
}

/**
 * Step forward, to compute the direct successors of tau by executing the thread
 * identified by tid. It returns a list of successors, aka. global states
 * @param q a shared state
 * @param W a stack for thread tid
 * @param tid thread ID
 * @param successors to store all successors
 */
void explicit_cuba::step(const pda_state& q, const stack_vec& W, const uint tid,
		antichain& successors) {
	if (tid >= CPDA.size() || W[tid].empty())
		return;
	/// step 2.2: iterator over all successors of current thread state
	const auto ifind = CPDA[tid].get_program().find(
			thread_visible_state(q, W[tid].top()));
	if (ifind == CPDA[tid].get_program().end())
		return;
	for (const auto rid : ifind->second) { /// rid: transition id
		const auto& r = CPDA[tid].get_actions()[rid];
		const auto& dst = r.get_dst();

		auto _W = W; /// duplicate the stacks in current global conf.

		switch (r.get_oper_type()) {
		case type_stack_operation::PUSH: { /// push operation
			_W[tid].pop();
			for (auto is = dst.get_stack().get_worklist().rbegin();
					is != dst.get_stack().get_worklist().rend(); ++is) {
				_W[tid].push(*is);
			}
			successors.emplace_back(tid, dst.get_state(), _W);
		}
			break;
		case type_stack_operation::POP: { /// pop operation
			if (_W[tid].pop())
				successors.emplace_back(tid, dst.get_state(), _W);
		}
			break;
		default: { /// overwrite operation
			if (_W[tid].overwrite(dst.get_stack().top()))
				successors.emplace_back(tid, dst.get_state(), _W);
		}
			break;
		}
	}
}

/**
 * Insert explicit_state_tid c to the set of reachable global states R.
 * It returns false if explicit_state_tid c has been reached, and true
 * otherwise.
 * @param R
 * @param k
 * @param c
 * @return bool
 */
bool explicit_cuba::update_R(vector<vector<antichain>>& R, const size_k k,
		const explicit_state_tid& c) {
	/// step 2.1.2: discard it if tau is already explored
	if (is_reachable(c, k, R))
		return false;
	if (k >= R.size())
		R.emplace_back(vector<antichain>(thread_visible_state::S));
	R[k][c.get_state()].emplace_back(c);
	return true;
}

/**
 * Determine whether reaching a convergence in k contexts. It returns true
 * if converges, false otherwise.
 * @param R_k  the global states reached in kth context.
 * @param k    current context is k
 * @param top_R the set of reachable visible states
 * @return bool
 */
bool explicit_cuba::converge(const vector<antichain>& R_k, const size_k k,
		vector<set<visible_state>>& top_R) {
	cout << "======================================\n";
	cout << "context " << k << "\n";
	/// the number of new reachable top states
	uint cnt_new_top_cfg = 0;
	for (uint q = 0; q < (uint) thread_visible_state::S; ++q) {
		for (const auto& c : R_k[q]) {
			if (prop::OPT_PRINT_ALL)
				cout << string(2, ' ') << c;
			const auto& top_c = top_mapping(c);

			/// if reachability and the target visible state is reachable
			if (prop::OPT_PROB_REACHABILITY && top_c == final_c) {
				reachable = true;
			}
			const auto& ret = top_R[c.get_state()].emplace(top_c);
			if (ret.second) {
				if (prop::OPT_PRINT_ALL)
					cout << " : " << top_c;
				/// find a new top state
				++cnt_new_top_cfg;
				/// updating approx_X
				auto ifind = generators[top_c.get_state()].find(top_c);
				if (ifind != generators[top_c.get_state()].end())
					generators[top_c.get_state()].erase(ifind);
			}
			if (prop::OPT_PRINT_ALL)
				cout << "\n";
		}
	}
	cout << "the number of new visible states: " << cnt_new_top_cfg << "\n";
	if (cnt_new_top_cfg == 0) {
		if (is_convergent()) {
			return true;
		}
		cout << "=> sequence T(R) plateaus at " << k << "\n";
	}
	return false;
}

/**
 * Determine if OS3 converges or not. Return true if OS3 converges, and
 * false otherwise.
 * @return bool
 */
bool explicit_cuba::is_convergent() {
	for (const auto& s : generators) {
		if (!s.empty())
			return false;
	}
	return true;
}

/**
 * This procedure is to determine whether there exists c \in R such that
 * (1) c.id == tau.id /\ c.s == c.s /\ c.W == tau.W /\ c.k <= tau.k.
 * It returns true if any of above conditions satisfies. Otherwise, it
 * returns false.
 *
 * (2) One special case is that: there exists c \in R such that
 * c.id == tau.id /\ c.s == tau.s /\ c.W == tau.W /\ c.k > tau.k,
 * then, c is removed.
 *
 * In the implementation, we first check condition (1) and then (2).
 *
 * @param tau
 * @param curr_k
 * @param R
 * @return bool
 */
bool explicit_cuba::is_reachable(const explicit_state_tid& tau,
		const size_k curr_k, vector<vector<antichain>>& R) {
	/// step 1: retrieve the shared state of tau
	const auto& q = tau.get_state();
	/// step 2: check condition 3
	for (uint j = curr_k + 1; j < R.size(); ++j) {
		for (uint i = 0; i < R[j][q].size(); ++i) {
			if (R[j][q][i] == tau)
				R[j][q].erase(R[j][q].begin() + i);
		}
	}

	/// step 3: check condition (1) and (2)
	for (uint j = 0; j <= curr_k && j < R.size(); ++j) {
		for (uint i = 0; i < R[j][q].size(); ++i) {
			if (R[j][q][i] == tau)
				return true;
		}
	}
	return false;
}

/**
 * Mark the visible thread state (s,l) as reachable
 * @param s
 * @param l
 */
void explicit_cuba::marking(const pda_state& s, const pda_alpha& l) {
	if (!reachable_T[s][l])
		reachable_T[s][l] = true;
}

/**
 * Extract tau's visible state
 * @param tau
 * @return visible_state
 */
visible_state explicit_cuba::top_mapping(const explicit_state_tid& tau) {
	vector<pda_alpha> W(tau.get_stacks().size());
	for (size_n i = 0; i < tau.get_stacks().size(); ++i) {
		if (tau.get_stacks()[i].empty())
			W[i] = alphabet::EPSILON;
		else
			W[i] = tau.get_stacks()[i].top();
	}
	return visible_state(tau.get_state(), W);
}

/**
 * This procedure implements the finite context reachability.
 * @param tid
 * @return bool
 */
bool explicit_cuba::finite_context_reachability(const size_n tid) {
	map<thread_visible_state, bool> visit;
	map<thread_visible_state, bool> trace;
	for (const auto& p : CPDA[tid].get_program()) {
		visit[p.first] = false;
		trace[p.first] = false;
	}

	for (const auto& p : CPDA[tid].get_program()) {
		stack<pda_alpha> W;
		W.push(p.first.get_alpha());
		if (!visit[p.first]
				&& finite_context_reachability(tid, p.first, W, visit, trace)) {
			return true;
		}
	}
	return false;
}

/**
 * A subrontine of FCR: call by finite_context_reachability(const size_n tid).
 * @param tid
 * @param c
 * @param visit
 * @param trace
 * @return bool
 */
bool explicit_cuba::finite_context_reachability(const size_n tid,
		const thread_visible_state& s, stack<pda_alpha>& W,
		map<thread_visible_state, bool>& visit,
		map<thread_visible_state, bool>& trace) {
	visit[s] = true;
	trace[s] = true;
	auto ifind = CPDA[tid].get_program().find(s);
	if (ifind != CPDA[tid].get_program().end()) {
		for (const auto rid : ifind->second) {
			const auto& r = CPDA[tid].get_actions()[rid];
			const auto& dst = r.get_dst();
			W.pop();
			switch (r.get_oper_type()) {
			case type_stack_operation::PUSH: {
				W.push(dst.get_stack().get_worklist()[1]);
				W.push(dst.get_stack().get_worklist()[0]);
			}
				break;
			case type_stack_operation::OVERWRITE: {
				W.push(dst.get_stack().top());
			}
				break;
			default: {
			}
				break;
			}
			if (W.empty())
				continue;
			thread_visible_state t(dst.get_state(), W.top());
			if (!visit[t]
					&& finite_context_reachability(tid, t, W, visit, trace)) {
				return true;
			} else if (trace[t]) {
				if (W.size() > 1)
					return true;
			}
		}
	}
	trace[s] = false;
	return false;
}

} /* namespace cuba */

