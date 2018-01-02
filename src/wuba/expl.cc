/**
 * Explicit write-(un)bounded analysis
 *
 * @date  : Jan 1, 2018
 * @author: Peizun Liu
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
		const string& filename, const size_n n_bound) :
		base_wuba(initl, final, filename, n_bound) {

}

/**
 * Destructor
 */
explicit_wuba::~explicit_wuba() {
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
		const explicit_config& c_I) {
	deque<explicit_config> currRound;
	currRound.emplace_back(c_I);
	size_k k = 0;
	/// <global_R>: the sequences of reachable global configurations. It's
	/// a 2-D vector where the row represents the round w.r.t. the resource
	/// while the column represents the shared state.
	vector<vector<deque<explicit_config>>> global_R;
	global_R.reserve(k_bound + 1);
	global_R.emplace_back(vector<deque<explicit_config>>(thread_state::S));
	global_R[k][c_I.get_state()].emplace_back(c_I);

	/// <visible_R>: the sequences of set of visible configurations. We
	/// compute the sequence from R directly.
	vector<set<top_config>> visible_R(thread_state::S);

	/// step 2: compute all reachable configurations with up to k_bound write
	while (k_bound == 0 || k <= k_bound) {
		deque<explicit_config> nextRound;

		/// step 2.1: compute nextLevel, or R_{k+1}: iterate over
		while (!currRound.empty()) {
			const auto tau = currRound.front();
			currRound.pop_front();

			const auto& k_images = step(tau);
			for (const auto& _tau : k_images) {
				if (!update_R(global_R, k + 1, tau))
					continue;

				/// add the successors to current round (R_{k}) if no write, and
				/// to next round (R_{k+1}) otherwise.
				_tau.get_state() == tau.get_state() ?
						currRound.emplace_back(_tau) :
						nextRound.emplace_back(_tau);
			}
		}

		if (nextRound.size() == 0) {
			cout << "=> sequence R and T(R) collapses at "
					<< (k == 0 ? k : k - 1) << "\n";
			cout << "======================================" << endl;
			return true;
		}

		/// step 2.3: if all configurations in current round have been processed,
		/// then move onto next round, aka with k+1 writes
		currRound.swap(nextRound), ++k;
	}

	return false;
}

deque<explicit_config> explicit_wuba::step(const explicit_config& tau) {
	deque<explicit_config> successors;
	const auto& q = tau.get_state();
	const auto& W = tau.get_stacks();
	for (size_n tid = 0; tid < W.size(); ++tid) {

	}
	return successors;
}

} /* namespace wuba */
