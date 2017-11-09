/**
 * @brief explore.cc
 *
 * @date  : Sep 28, 2016
 * @author: lpzun
 */

#include "cuba.hh"

namespace cuba {
/**
 * constructor
 * @param initl initial global state
 * @param final final global state
 * @param filename: input CPDs
 * @param n: the number of threads, for parameterized system only
 */
explicit_cuba::explicit_cuba(const string& initl, const string& final,
		const string& filename, const size_t n) :
		base_cuba(initl, final, filename, n) {
}

/**
 * destructor
 */
explicit_cuba::~explicit_cuba() {

}

/**
 * The procedure of context-bounded analysis
 * @param n: the number of threads
 * @param k: the number of context switches
 */
void explicit_cuba::context_unbounded_analysis(const size_k k_bound) {
//	bool cycle = false;
//	for (size_t tid = 0; tid < CPDA.size(); ++tid) {
//		if (finite_context_reachability(tid)) {
//			cycle = true;
//			cout << "PDA " << tid << " contains cycles. Simulator exits...\n";
//			break;
//		}
//	}
//	cout << "Acycle.......\n";
//	if (cycle)
//		return;
	const auto is_reachable = k_bounded_reachability(k_bound, initl_c);
	if (prop::OPT_PROB_REACHABILITY && is_reachable) {
		cout << final_c << " is reachable!" << endl;
	}
}

/**
 * This procedure is to compute the set of reachable thread/global states
 * of a program P running by n threads and with up to k context switches.
 * @param n: the number of threads
 * @param k: the upper bound of context switches
 * @return the number of reachable thread/global states
 */
bool explicit_cuba::k_bounded_reachability(const size_k k_bound,
		const explicit_config& c_I) {
	/// step 1: set the data structures that used in the explicit exploration
	///         procedure
	/// 1.1 <currLevel> = R_{k} \ R_{k-1}: the set of unexplored global conf-
	/// igurations which are reached in the kth context for processing. It's
	/// initialized as { <q_I | (w1_I, ..., wn_I> }
	antichain currLevel;
	currLevel.emplace_back(CPDA.size(), c_I.get_state(), c_I.get_stacks());
	/// 1.2 <k>: contexts, k = 0 represented the initial configurations c_I
	size_k k = 0;
	/// 1.3 <global_R>: the sequences of reachable global configurations.
	/// We obtain this via a BWS based explicit exploration.
	vector<vector<antichain>> global_R;

	global_R.reserve(k_bound + 1);
	global_R.emplace_back(vector<antichain>(thread_state::S));
	global_R[k][c_I.get_state()].emplace_back(CPDA.size(), c_I.get_state(),
			c_I.get_stacks());
	/// 1.4 <top_R>: the sequences of top configurations. We obtained the
	/// sequence from R directly.
	vector<set<top_config>> top_R(thread_state::S);
	/// step 2: compute all reachable configurations with up to k_bound contexts.
	while (k_bound == 0 || k <= k_bound) {
		/// step 2.0 <nextLevel> = R_{k+1} \ R_{k}: the set of explicit
		/// configurations reached in the (k+1)st context. It's initialized
		/// as empty.
		antichain nextLevel;
		/// step 2.1: compute nextLevel, or R_{k+1}: iterate over all elements
		/// in the currLevel. This is a BFS-like procedure.
		while (!currLevel.empty()) {
			/// 2.1.1: remove an element from currLevel
			const auto c = currLevel.front();
			currLevel.pop_front();
			/// 2.1.3: compute its successors and process them one by one
			const auto& k_images = step(c, false);
			for (const auto& _c : k_images) {
				/// 2.1.4: skip _c if it's already been reached
				if (!update_R(global_R, k, _c))
					continue;
				/// 2.1.5: add _c to worklist if it's unreachable
				currLevel.emplace_back(_c);
			}
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
			cout << "=> sequence R collapses at " << (k == 0 ? k : k - 1)
					<< "\n";
			cout << "======================================" << endl;
			return true;
		}

		if (converge(global_R[k], k, top_R)) {
			cout << "=> sequence top_R collapses at " << (k == 0 ? k : k - 1)
					<< "\n";
			cout << "======================================" << endl;
			return true;
		}
		/// step 2.3: if all elements in currLevel has been processed, then
		/// move onto the (k+1)st contexts.
		currLevel.swap(nextLevel), ++k;
	}

	return false; /// return the number of reachable thread states
}

/**
 * step forward, to compute the direct successors of tau
 * @param tau: a global configuration
 * @return a list of successors, aka. global configurations
 */
antichain explicit_cuba::step(const global_config& tau, const bool is_switch) {
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
			/// @WARNING: this means we don't allow an action whose source
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
 *
 * @param q
 * @param W
 * @param tid
 * @param successors
 */
void explicit_cuba::step(const pda_state& q, const stack_vec& W, const uint tid,
		antichain& successors) {
	if (tid >= CPDA.size() || W[tid].empty())
		return;
	/// step 2.2: iterator over all successor of current thread state
	const auto& ifind = CPDA[tid].get_program().find(
			thread_state(q, W[tid].top()));
	if (ifind == CPDA[tid].get_program().end())
		return;
	for (const auto& rid : ifind->second) { /// rid: transition id
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
 *
 * @param R
 * @param k
 * @param c
 * @return
 */
bool explicit_cuba::update_R(vector<vector<antichain>>& R, const size_k k,
		const global_config& c) {
	/// step 2.1.2: discard it if tau is already explored
	if (is_reachable(c, k, R))
		return false;
	if (k >= R.size())
		R.emplace_back(vector<antichain>(thread_state::S));
	R[k][c.get_state()].emplace_back(c);
	return true;
}

/**
 * Determine whether reaching a convergence in k contexts
 * @param R
 * @return
 */
bool explicit_cuba::converge(const vector<antichain>& R_k, const size_k k,
		vector<set<top_config>>& top_R) {
	cout << "======================================\n";
	cout << "context " << k << "\n";
	/// the number of new reachable top configurations
	uint cnt_new_top_cfg = 0;
	for (uint q = 0; q < thread_state::S; ++q) {
		for (const auto& c : R_k[q]) {
			cout << string(2, ' ') << c;
			const auto& top_c = top_mapping(c);
			const auto& ret = top_R[c.get_state()].emplace(top_c);
			if (ret.second) {
				cout << " : " << top_c;
				/// find a new top configuration
				++cnt_new_top_cfg;
				/// updating approx_X
				auto ifind = generators[top_c.get_state()].find(top_c);
				if (ifind != generators[top_c.get_state()].end())
					generators[top_c.get_state()].erase(ifind);
			}
			cout << "\n";
		}
	}
	if (cnt_new_top_cfg == 0) {
		if (is_convergent()) {
			return true;
		}
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
bool explicit_cuba::is_convergent() {
	for (const auto& s : generators) {
		if (!s.empty())
			return false;
	}
	return true;
}

/**
 * This procedure is to determine whether there exists c \in R such that
 * (1) c == tau;
 * (2) c.id == tau.id /\ c.s == c.s /\ c.W == tau.W /\ c.k < tau.k.
 * It returns true if any of above conditions satisfies. Otherwise, it
 * returns false.
 *
 * (3) One special case is that: there exists c \in explored such that
 * c.id == tau.id /\ c.s == tau.s /\ c.W == tau.W /\ c.k > tau.k,
 * then, the procedure replaces c by tau except returning false.
 *
 * @param tau
 * @param curr_k
 * @param R
 * @return bool
 */
bool explicit_cuba::is_reachable(const global_config& tau, const size_k curr_k,
		vector<vector<antichain>>& R) {
	bool flag = false;

	const auto& q = tau.get_state();
	/// step 1: check condition (1) and (2)
	for (uint k = 0; k <= curr_k && k < R.size(); ++k) {
		for (uint i = 0; i < R[k][q].size(); ++i) {
			if (R[k][q][i] == tau) {
				flag = true;
				break; /// break out the inner loop
			}
		}
		if (flag) /// break out the outer loop
			break;
	}
	/// step 2: check condition 3
	for (uint k = curr_k + 1; k < R.size(); ++k) {
		for (uint i = 0; i < R[k][q].size(); ++i) {
			if (R[k][q][i] == tau)
				R[k][q].erase(R[k][q].begin() + i);
		}
	}
	return flag;
}

/**
 * set thread state (s,l) as reachable
 * @param s
 * @param l
 */
void explicit_cuba::marking(const pda_state& s, const pda_alpha& l) {
	if (!reachable_T[s][l])
		reachable_T[s][l] = true;
}

/**
 * Obtain the top of configuration
 * @param tau
 * @return
 */
top_config explicit_cuba::top_mapping(const global_config& tau) {
	vector<pda_alpha> W(tau.get_stacks().size());
	for (size_t i = 0; i < tau.get_stacks().size(); ++i) {
		if (tau.get_stacks()[i].empty())
			W[i] = alphabet::EPSILON;
		else
			W[i] = tau.get_stacks()[i].top();
	}
	return top_config(tau.get_state(), W);
}

/**
 * Cycle detection
 * @param tid
 * @return bool
 */
bool explicit_cuba::finite_context_reachability(const size_t tid) {
	map<vertex, bool> visit;
	map<vertex, bool> trace;
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
 *
 * @param tid
 * @param c
 * @param visit
 * @param trace
 * @return
 */
bool explicit_cuba::finite_context_reachability(const size_t tid,
		const thread_state& s, stack<pda_alpha>& W,
		map<thread_state, bool>& visit, map<thread_state, bool>& trace) {
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
			thread_state t(dst.get_state(), W.top());
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

