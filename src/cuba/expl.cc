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
simulator::simulator(const string& initl, const string& final,
		const string& filename) :
		initl_c(0, 1), final_c(0, 1), CPDA(), reachable_T() {
	initl_c = parser::parse_input_cfg(initl);
	final_c = parser::parse_input_cfg(final);
	CPDA = parser::parse_input_cpds(filename);
}

/**
 * destructor
 */
simulator::~simulator() {

}

/**
 * The procedure of context-bounded analysis
 * @param n: the number of threads
 * @param k: the number of context switches
 */
void simulator::context_bounded_analysis(const size_k& k, const size_t& n) {
	bool cycle = false;
	for (size_t tid = 0; tid < CPDA.size(); ++tid) {
		if (is_cyclic(tid)) {
			cycle = true;
			cout << "PDA " << tid << " contains cycles. Simulator exits...\n";
			break;
		}
	}
	if (cycle)
		return;
	const auto is_reachable = k_bounded_reachability(k, initl_c);
	if (is_reachable) {
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
bool simulator::k_bounded_reachability(const size_k k_bound,
		const concrete_config& c_I) {
	/// step 1: build the initial  global configuration
	///         Here we build the set of initial stacks

	/// step 2: set the data structures that used in the exploration
	///         procedure
	/// the set of unexplored global configurations
	antichain worklist;
	/// set the initial global configuration
	worklist.emplace_back(CPDA.size(), 0, c_I.get_state(), c_I.get_stacks());
	/// the set of reachable global configurations
	vector<vector<antichain>> global_R(k_bound + 1,
			vector<antichain>(thread_state::S));
	vector<set<config_top>> topped_R(thread_state::S);

	/// step 3: the exploration procedure: it is based on BFS
	while (!worklist.empty()) {
		/// step 3.1: remove an element from currLevel
		const auto tau = worklist.front();
		worklist.pop_front();
		// cout << tau << "\n"; /// deleting ---------------------

		/// step 3.2: discard it if tau is already explored
		if (is_reachable(tau, global_R))
			continue;
		//cout << tau << "\n"; /// deleting ---------------------

		/// step 3.3: compute its successors and process them
		///           one by one
		const auto& images = step(tau, k_bound);
		for (const auto& _tau : images) {
			//cout << string(3, ' ') << _tau << "\n"; // todo deleting ---------------------
			worklist.emplace_back(_tau);
		}
		/// step 3.4: add current configuration to global_R set
		global_R[tau.get_context_k()][tau.get_state()].emplace_back(tau);
	}

	cout << "======================================\n";
	for (uint k = 0; k < global_R.size(); ++k) {
		cout << "context " << k << "\n";
		for (uint q = 0; q < global_R[q].size(); ++q) {
			for (const auto& c : global_R[k][q]) {
				cout << string(2, ' ') << c;
				const auto& cbar = top_mapping(c);
				//cout << c << " .... " << cbar << endl; // todo deleting -------------
				const auto& ret = topped_R[c.get_state()].emplace(cbar);
				if (ret.second)
					cout << " | " << cbar;
				cout << "\n";
			}
		}
	}
	cout << "======================================\n";
	return false; /// return the number of reachable thread states
}

/**
 * This procedure is to determine whether there exists c \in R such that
 *  (1) c == tau;
 *  (2) c.id == tau.id /\ c.s == c.s /\ c.W == tau.W /\ c.k < tau.k.
 * It returns true if any of above conditions satisfies. Otherwise, it returns
 * false.
 *
 * One special case is that: there exists c \in explored such that
 *    c.id == tau.id /\ c.s == tau.s /\ c.W == tau.W /\ c.k > tau.k,
 * then, the procedure replaces c by tau except returning false.
 *
 * @param tau
 * @param R
 * @return bool
 */
bool simulator::is_reachable(const global_config& tau,
		vector<vector<antichain>>& R) {
	bool flag = false;
	for (uint k = 0; k <= tau.get_context_k(); k++) {
		for (const auto& c : R[k][tau.get_state()]) {
			if (c == tau) {
				flag = true;
				break;
			}
		}
	}

	for (uint k = tau.get_context_k() + 1; k < R.size(); ++k) {
		for (auto it = R[k][tau.get_state()].begin();
				it != R[k][tau.get_state()].end(); ++it) {
			R[k][tau.get_state()].erase(it);
		}
	}
	return flag;
}

/**
 * step forward, to compute the direct successors of tau
 * @param tau: a global configuration
 * @return a list of successors, aka. global configurations
 */
antichain simulator::step(const global_config& tau, const size_k k_bound) {
	antichain successors;
	const auto& k = tau.get_context_k(); /// the context switches to tau

	/// step 1: if context switches already reach to the upper bound
	const auto& q = tau.get_state();     /// the control state of tau
	const auto& W = tau.get_stacks();    /// the stacks of tau

	/// step 2: iterate over all threads
	for (uint tid = 0; tid < W.size(); ++tid) {
		//cout << tid << "\\\\\\\\\\\\\\\\\\\\\\\\\n";
		if (W[tid].empty() || (k == k_bound && tid != tau.get_thread_id())) {
			continue;
		}
		/// step 2.1: set context switches
		///           if context switch occurs, then k = k + 1;
		auto _k = tid == tau.get_thread_id() ? k : k + 1;
		/// step 2.2: iterator over all successor of current thread state

		//cout << q << "," << W[tid].top() << "\\\\\\\\\\\\\\\\\\\\\\\\\n";
		const auto& ifind = CPDA[tid].get_pda().find(
				thread_state(q, W[tid].top()));
		if (ifind == CPDA[tid].get_pda().end())
			continue;
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
				successors.emplace_back(tid, _k, dst.get_state(), _W);
			}
				break;
			case type_stack_operation::POP: { /// pop operation
				if (_W[tid].pop())
					successors.emplace_back(tid, _k, dst.get_state(), _W);
			}
				break;
			default: { /// overwrite operation
				if (_W[tid].overwrite(dst.get_stack().top()))
					successors.emplace_back(tid, _k, dst.get_state(), _W);
			}
				break;
			}
		}
	}
	return successors; /// the set of successors of tau
}

/**
 * set thread state (s,l) as reachable
 * @param s
 * @param l
 */
void simulator::marking(const pda_state& s, const pda_alpha& l) {
	if (!reachable_T[s][l])
		reachable_T[s][l] = true;
}

/**
 * Obtain the top of configuration
 * @param global_R
 * @return
 */
config_top simulator::top_mapping(const global_config& tau) {
	vector<pda_alpha> L(tau.get_stacks().size());
	for (size_t i = 0; i < tau.get_stacks().size(); ++i) {
		if (tau.get_stacks()[i].empty())
			L[i] = alphabet::EPSILON;
		else
			L[i] = tau.get_stacks()[i].top();
	}
	return config_top(tau.get_state(), L);
}

/**
 * Cycle detection
 * @param tid
 * @return bool
 */
bool simulator::is_cyclic(const size_t tid) {
	map<vertex, bool> visit;
	map<vertex, bool> trace;
	for (const auto& p : CPDA[tid].get_pda()) {
		visit[p.first] = false;
		trace[p.first] = false;
	}

	for (const auto& p : CPDA[tid].get_pda()) {
		stack<pda_alpha> W;
		W.push(p.first.get_symbol());
		if (!visit[p.first] && is_cyclic(tid, p.first, W, visit, trace)) {
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
bool simulator::is_cyclic(const size_t tid, const thread_state& s,
		stack<pda_alpha>& W, map<thread_state, bool>& visit,
		map<thread_state, bool>& trace) {
	visit[s] = true;
	trace[s] = true;
	auto ifind = CPDA[tid].get_pda().find(s);
	if (ifind != CPDA[tid].get_pda().end()) {
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
			if (!visit[t] && is_cyclic(tid, t, W, visit, trace)) {
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

