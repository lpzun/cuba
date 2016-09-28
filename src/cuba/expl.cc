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
 * @param k     context switch bound
 * @param initl initial thread state
 * @param final final thread state
 * @param mapping_Q: mapping a control state to its ID
 * @param Q: active control states
 * @param R: active transitions
 */
explore::explore(const ctx_bound& k, const thread_state& initl,
		const thread_state& final,  ///
		const vector<vector<vertex>>& mapping_Q, ///
		const vector<thread_state>& Q, ///
		const vector<pda_transition<vertex>>& R) :
		k_bound(k), initl_TS(initl), final_TS(final), ///
		mapping_Q(mapping_Q), active_Q(Q), active_R(R) {

}

/**
 * destructor
 */
explore::~explore() {

}

/**
 * This procedure is to compute the set of reachable thread/global states
 * of a program P running by n threads and with up to k context switches.
 * @param n: the number of threads
 * @param k: the upper bound of context switches
 * @return the number of reachable thread/global states
 */
uint explore::bounded_reachability(const size_t& n, const size_k& k) {
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
bool explore::is_reachable(const global_config& tau, antichain& R) {
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
antichain explore::step(const global_config& tau) {
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
vertex explore::retrieve(const pda_state& s, const pda_alpha& l) {
	return mapping_Q[s][l];
}

/**
 * set thread state (s,l) as reachable
 * @param s
 * @param l
 */
void explore::marking(const pda_state& s, const pda_alpha& l) {
	if (!reachable_T[s][l])
		reachable_T[s][l] = true;
}

} /* namespace cuba */

