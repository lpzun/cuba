/**
 * @brief pre_processor.cc
 *
 * @date  : Jun 13, 2017
 * @author: Peizun Liu
 */
#include "cuba.hh"

namespace cuba {
/**
 * A constructor with initial top configuration and a concurrent finite machine
 * @param c_I : an initial top configuration
 * @param cfsm: concurrent finite machine
 */
processor::processor(const top_config& c_I,
		const concurrent_finite_machine& cfsm) :
		initl_c(c_I), cfsm(cfsm) {
}

/**
 * A default destructor
 */
processor::~processor() {

}

vector<set<top_config>> processor::over_approx_top_R() {
	return standard_FWS();
}

/**
 * Standard forward search: to compute the overapproximation of the set of
 * reachable top configurations.
 *
 * @return a set of reachable top configurations
 */
vector<set<top_config>> processor::standard_FWS() {
	/// step 1: set up the data structures
	/// 1.1  <approx_R>: the  overapproximation of the set of reachable
	///      top configurations
	vector<set<top_config>> approx_R(thread_state::S);
	vector<set<top_config>> approx_X(thread_state::S);
	/// 1.2 <worklist>: a worklist
	queue<top_config> worklist;
	worklist.emplace(initl_c);

	while (!worklist.empty()) {
		const auto c = worklist.front();
		worklist.pop();

		const auto ret = approx_R[c.get_state()].emplace(c);
		if (!ret.second)
			continue;
		const auto& successors = step(c, approx_X);
		for (const auto& _c : successors) {
			worklist.emplace(_c);
		}
	}

	// delete------------
	cout << "Approximation Z:\n";
	for (const auto& v : approx_R) {
		for (const auto& c : v)
			cout << c << endl;
	}
	cout << "Approximation X:\n";
	for (const auto& v : approx_X) {
		for (const auto& c : v)
			cout << c << endl;
	}
	// delete------------
	return approx_R;
}

/**
 *
 * @param c
 * @return
 */
deque<top_config> processor::step(const top_config& c,
		vector<set<top_config>>& approx_X) {
	deque<top_config> successors;
	for (uint i = 0; i < c.get_local().size(); ++i) {
		thread_state src(c.get_state(), c.get_local()[i]);
		auto ifind = cfsm[i].find(src);
		if (ifind == cfsm[i].end())
			continue;
		for (const auto& trans : ifind->second) {
			const auto& dst = trans.get_dst();
			auto local(c.get_local());
			local[i] = dst.get_alpha();
			successors.emplace_back(dst.get_state(), local);
			if (trans.get_oper_type() == type_stack_operation::POP) {
				approx_X[c.get_state()].emplace(dst.get_state(), local);
			}
		}
	}
	return successors;
}

} /* namespace cuba */

