/**
 * @brief pre_processor.cc
 *
 * @date  : Jun 13, 2017
 * @author: Peizun Liu
 */
#include <cuba.hh>

namespace cuba {
/**
 * A constructor with initial top configuration and a concurrent finite machine
 * @param c_I : an initial top configuration
 * @param cfsm: concurrent finite machine
 */
processor::processor(const string& initl, const string& final,
		const string& filename) :
		initl_c(0, 1), final_c(0, 1), CFSM() {
	initl_c = top_mapping(parser::parse_input_cfg(initl));
	final_c = top_mapping(parser::parse_input_cfg(final));
	CFSM = parser::parse_input_cfsm(filename);
}

/**
 *
 * @param initl
 * @param final
 * @param filename
 */
processor::processor(const concrete_config& initl, const string& filename) :
		initl_c(top_mapping(initl)), final_c(0, 1), CFSM() {
	CFSM = parser::parse_input_cfsm(filename);
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
	/// 1.1 <approx_R>: the overapproximation of the set of reachable
	///     top configurations
	vector<set<top_config>> approx_x(thread_state::S);
	/// 1.2 <approx_R>: the overapproximation of the set of reachable
	///     popped top configurations
	vector<set<top_config>> approx_X(thread_state::S);
	/// 1.3 <worklist>: a worklist
	queue<top_config> worklist( { initl_c });

	while (!worklist.empty()) {
		const auto c = worklist.front();
		worklist.pop();

		const auto& ret = approx_x[c.get_state()].emplace(c);
		if (!ret.second)
			continue;
		const auto& successors = step(c, approx_X);
		for (const auto& _c : successors) {
			worklist.emplace(_c);
		}
	}

	// delete-----------------------
	cout << "Approximation Z:\n";
	for (const auto& v : approx_x) {
		if (v.size() == 0)
			continue;
		cout << "  ";
		for (const auto& c : v)
			cout << c << " ";
		cout << "\n";
	}
	cout << "Approximation X:\n";
	for (const auto& v : approx_X) {
		if (v.size() == 0)
			continue;
		cout << "  ";
		for (const auto& c : v)
			cout << c << " ";
		cout << "\n";
	}
	// delete-----------------------
	return approx_X;
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
		auto ifind = CFSM[i].find(src);
		if (ifind == CFSM[i].end())
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

/**
 * Obtain the top of configuration
 * @param tau
 * @return
 */
top_config processor::top_mapping(const concrete_config& tau) {
	vector<pda_alpha> L(tau.get_stacks().size());
	for (size_t i = 0; i < tau.get_stacks().size(); ++i) {
		if (tau.get_stacks()[i].empty())
			L[i] = alphabet::EPSILON;
		else
			L[i] = tau.get_stacks()[i].top();
	}
	return top_config(tau.get_state(), L);
}

} /* namespace cuba */

