/**
 * Explicit write-(un)bounded analysis
 *
 * @date   Jan 1, 2018
 * @author TODO
 */
#include "wuba.hh"

namespace wuba {

/**
 * Constructor
 * @param initl
 * @param final
 * @param filename
 * @param n
 */
explicit_wuba::explicit_wuba(const string& initl, const string& final,
		const string& filename) :
		base_wuba(initl, final, filename) {

}

/**
 * Destructor
 */
explicit_wuba::~explicit_wuba() {
}

void explicit_wuba::write_bounded_analysis(const size_k k_bound) {
	cout << "input " << k_bound << " " << initl_c << " " << final_c << endl;
	k_bounded_reachability(k_bound, initl_c);
}

void explicit_wuba::write_unbounded_analysis() {

}

/**
 * This procedure is to compute the set of reachable thread/global states
 * of a concurrent program P with up to k writes
 * @param k_bound: the upper bound of write accesses
 * @param c_I    : the initial global configurations
 * @return bool
 *         true : converge
 *         false: all bad states are unreachable
 */
bool explicit_wuba::k_bounded_reachability(const size_k k_bound,
		const explicit_state& c_I) {
	deque<explicit_state> currRound;
	currRound.emplace_back(c_I);
	size_k k = 0;
	/// <global_R>: the sequences of reachable global configurations. It's
	/// a 2-D vector where the row represents the round w.r.t. the resource
	/// while the column represents the shared state.
	vector<vector<deque<explicit_state>>> global_R;
	global_R.reserve(k_bound + 1);
	global_R.emplace_back(
			vector<deque<explicit_state>>(thread_visible_state::S));
	global_R[k][c_I.get_state()].emplace_back(c_I);

	/// <visible_R>: the sequences of set of visible configurations. We
	/// compute the sequence from R directly.
	vector<set<visible_state>> top_R(thread_visible_state::S);

	/// step 2: compute all reachable configurations with up to k_bound write
	while (k_bound == 0 || k <= k_bound) {
		deque<explicit_state> nextRound;

		/// step 2.1: compute nextLevel, or R_{k+1}: iterate over
		while (!currRound.empty()) {
			const auto tau = currRound.front();
			currRound.pop_front();

			const auto& k_images = step(tau);
			for (const auto& _tau : k_images) {
				/// add the successors to current round (R_{k}) if no write,
				/// and to next round (R_{k+1}) otherwise.
				if (_tau.get_state() == tau.get_state()) {
					if (!update_R(_tau, k, global_R))
						continue;
					currRound.emplace_back(_tau);
				} else {
					if (!update_R(_tau, k + 1, global_R))
						continue;
					nextRound.emplace_back(_tau);
				}
			}
		}

		/// step 2.2: convergence detection
		/// 2.2.1: global_R collapses
		if (convergence_GS > 0 || nextRound.size() == 0) {
			cout << "=> sequence R and T(R) collapses at ";
			if (convergence_GS == 0)
				convergence_GS = k == 0 ? k : k - 1;
			cout << convergence_GS << "\n";
			cout << "======================================" << endl;
		}
		/// 2.2.2: top_R collapses
		if (converge(global_R[k], k, top_R) || convergence_VS) {
			cout << "=> sequence T(R) collapses at ";
			if (convergence_VS == 0)
				convergence_VS = k == 0 ? k : k - 1;
			cout << convergence_VS << "\n";
			cout << "======================================" << endl;
		}

		/// k_bound == 0 means the user does specify the resource
		/// bound, so unbounded analysis applies. We need to return
		/// when we reach a convergence.
		if (k_bound == 0 && (convergence_GS > 0 || convergence_VS > 0))
			return true;

		/// step 2.3: if all configurations in current round have been processed,
		/// then move onto next round, aka with k+1 writes
		currRound.swap(nextRound), ++k;
	}

	return false;
}

/**
 *
 * @param tau
 * @return
 */
deque<explicit_state> explicit_wuba::step(const explicit_state& tau) {
	deque<explicit_state> successors;
	const auto& q = tau.get_state();
	const auto& W = tau.get_stacks();
	for (size_n tid = 0; tid < W.size(); ++tid) {
		if (W[tid].empty())
			continue;
		const auto ifind = CPDA[tid].get_program().find(
				thread_visible_state(q, W[tid].top()));
		if (ifind == CPDA[tid].get_program().end())
			continue;
		for (const auto rid : ifind->second) { /// rid: transition id
			const auto& r = CPDA[tid].get_actions()[rid];
			const auto& dst = r.get_dst();
			auto _W = W;
			switch (r.get_oper_type()) {
			case type_stack_operation::PUSH: { /// push operation
				_W[tid].pop();
				for (auto is = dst.get_stack().get_worklist().rbegin(), ie =
						dst.get_stack().get_worklist().rend(); is != ie; ++is)
					_W[tid].push(*is);
				successors.emplace_back(dst.get_state(), _W);
			}
				break;
			case type_stack_operation::POP: { /// pop operation
				if (_W[tid].pop())
					successors.emplace_back(dst.get_state(), _W);
			}
				break;
			default: { /// overwrite operation
				if (_W[tid].overwrite(dst.get_stack().top()))
					successors.emplace_back(dst.get_state(), _W);
			}
				break;
			}
		}
	}
	return successors;
}

/**
 *
 * @param R
 * @param k
 * @param c
 * @return
 */
bool explicit_wuba::update_R(const explicit_state& tau, const size_k k,
		vector<vector<deque<explicit_state>>>& R) {
	/// step 2.1.2: discard it if tau is already explored
	if (is_reachable(tau, k, R))
		return false;
	if (k >= R.size())
		R.emplace_back(vector<deque<explicit_state>>(thread_visible_state::S));
	R[k][tau.get_state()].emplace_back(tau);
	return true;
}

/**
 * This procedure is to determine whether there exists c \in R such that
 * (1) c.s == c.s /\ c.W == tau.W /\ c.k <= tau.k.
 * It returns true if any of above conditions satisfies. Otherwise, it
 * returns false.
 *
 * (2) One special case is that: there exists c \in R such that
 * c.s == tau.s /\ c.W == tau.W /\ c.k > tau.k, then, c is removed.
 *
 * In the implementation, we first check condition (1) and then (2).
 *
 * @param tau
 * @param k
 * @param R
 * @return bool
 */
bool explicit_wuba::is_reachable(const explicit_state& tau, const size_k k,
		vector<vector<deque<explicit_state>>>& R) {
	/// step 1: retrieve the shared state of tau
	const auto& q = tau.get_state();
	/// step 2: check condition (2)
	for (size_k j = k + 1; j < R.size(); ++j) {
		for (auto it = R[j][q].begin(); it != R[j][q].end(); ++it) {
			if (*it == tau)
				R[j][q].erase(it);
		}
	}
	/// step 3: check condition (1)
	for (size_k j = 0; j <= k && j < R.size(); ++j) {
		for (auto it = R[j][q].begin(); it != R[j][q].end(); ++it) {
			if (*it == tau)
				return true;
		}
	}
	return false;
}

/**
 *
 * @param Rk
 * @param k
 * @param
 * @return
 */
bool explicit_wuba::converge(const vector<deque<explicit_state>>& Rk,
		const size_k k, vector<set<visible_state>>& top_R) {
	cout << "======================================\n";
	cout << "write " << k << "\n";
	/// the number of new reachable top configurations
	uint cnt_new_top_cfg = 0;
	for (pda_state q = 0; q < thread_visible_state::S; ++q) {
		for (const auto& c : Rk[q]) {
			if (flags::OPT_PRINT_ALL)
				cout << string(2, ' ') << c;
			const auto& top_c = top_mapping(c);
			const auto& ret = top_R[c.get_state()].emplace(top_c);
			if (ret.second) {
				if (flags::OPT_PRINT_ALL)
					cout << " : " << top_c;
				++cnt_new_top_cfg;
				/// removing reachable generators
				auto ifind = generators[top_c.get_state()].find(top_c);
				if (ifind != generators[top_c.get_state()].end())
					generators[top_c.get_state()].erase(ifind);
			}
			if (flags::OPT_PRINT_ALL)
				cout << "\n";
		}
	}
	cout << "the number of new visible states: " << cnt_new_top_cfg << "\n";
	if (cnt_new_top_cfg == 0) {
		if (converge())
			return true;
		cout << "=> sequence T(R) plateaus at " << k << "\n";
	}
	return false;
}

/**
 * Determine if OS3 converges or not
 * @return bool
 *         true : if OS3 converges
 *         false: otherwise
 */
bool explicit_wuba::converge() {
	for (const auto& s : generators) {
		if (!s.empty())
			return false;
	}
	return true;
}

/**
 *
 * @param tau
 * @return
 */
visible_state explicit_wuba::top_mapping(const explicit_state& tau) {
	vector<pda_alpha> W(tau.get_stacks().size());
	for (size_n i = 0; i < tau.get_stacks().size(); ++i)
		W[i] = tau.get_stacks()[i].empty() ?
				alphabet::EPSILON : tau.get_stacks()[i].top();
	return visible_state(tau.get_state(), W);
}

} /* namespace wuba */
