/**
 * @brief pre_processor.cc
 *
 * @date  : Jun 13, 2017
 * @author: Peizun Liu
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
base_cuba::base_cuba(const string& initl, const string& final,
		const string& filename, const size_t n) :
		initl_c(0, 1), final_c(0, 1), CPDA(), generators(), reachable_T() {
	if (prop::OPT_PARAMETERIZED) {

	} else {
		initl_c = parser::parse_input_cfg(initl);
		final_c = parser::parse_input_cfg(final);
		CPDA = parser::parse_input_cpds(filename);
	}

	/// set up overapproximation of reachable top configurations
	context_insensitive(initl_c, filename);
}

base_cuba::~base_cuba() {

}

/////////////////////////////////////////////////////////////////////////
/// PART 2. A preprocessor, overapproximate the set of reachable top
/// configurations
///
/////////////////////////////////////////////////////////////////////////

/**
 *
 * @param c_I : an initial top configuration
 * @param cfsm: concurrent finite machine
 */
void base_cuba::context_insensitive(const string& initl,
		const string& filename) {
	const auto& initl_c = top_mapping(parser::parse_input_cfg(initl));

	const auto& cfsm_for_Z = parser::parse_input_cfsm(filename);
	generators = vector<set<top_config>>(thread_state::S);

	const auto& approx_Z = context_insensitive(initl_c, cfsm_for_Z);

	const auto& cfsm_for_Y = parser::parse_input_cfsm(filename, true);
	const auto& approx_Y = context_insensitive(initl_c, cfsm_for_Y);

	cout << "Approximation Z:\n";
	this->print_approximation(approx_Z);
	cout << "Approximation Y:\n";
	this->print_approximation(approx_Y);
	cout << "G intersects Z:\n";
	this->print_approximation(generators);
}

/**
 * A constructor with initial top configuration and a concurrent finite machine
 *
 * @param initl   : initial configuration
 * @param filename: the file name for CPDS
 */
void base_cuba::context_insensitive(const explicit_config& initl,
		const string& filename) {
	const auto& top_initl = top_mapping(initl);
	const auto& CFSM = parser::parse_input_cfsm(filename);
	generators = vector<set<top_config>>(thread_state::S);
	const auto& approx_Z = context_insensitive(top_initl, CFSM);

	cout << "Approximation Z:\n";
	this->print_approximation(approx_Z);
	cout << "G intersects Z:\n";
	this->print_approximation(generators);

	if (prop::OPT_PRINT_ALL) {
		for (const auto& fsm : CFSM)
			for (const auto& adjlist : fsm)
				for (const auto& r : adjlist.second)
					cout << r << "\n";
	}
}

/**
 *
 * @param top_initl
 * @param CFSM
 * @return
 */
vector<set<top_config>> base_cuba::context_insensitive(
		const top_config& top_initl, const vector<finite_machine>& CFSM) {
	return standard_FWS(top_initl, CFSM);
}

/**
 * Standard forward search: to compute the overapproximation of the set of
 * reachable top configurations.
 *
 * @return a set of reachable top configurations
 */
vector<set<top_config>> base_cuba::standard_FWS(const top_config& initl_c,
		const vector<finite_machine>& CFSM) {
	/// step 1: set up the data structures
	/// 1.1 <approx_Z>: the overapproximation of the set of reachable
	///     top configurations
	vector<set<top_config>> approx_Z(thread_state::S);
	/// 1.2 <worklist>: a worklist
	queue<top_config> worklist( { initl_c });
	while (!worklist.empty()) {
		const auto c = worklist.front();
		worklist.pop();
		const auto& ret = approx_Z[c.get_state()].emplace(c);
		if (!ret.second)
			continue;
		for (const auto& _c : step(c, CFSM))
			worklist.emplace(_c);
	}
	return approx_Z;
}

/**
 * Compute the successors of top configuration c and update approx_X
 * @param c
 * @return return a list of top configurations
 */
deque<top_config> base_cuba::step(const top_config& c,
		const vector<finite_machine>& CFSM) {
	deque<top_config> successors;
	for (uint i = 0; i < c.get_local().size(); ++i) {
		auto ifind = CFSM[i].find(
				thread_state(c.get_state(), c.get_local()[i]));
		if (ifind == CFSM[i].end())
			continue;
		for (const auto& trans : ifind->second) {
			/// set up successors' control states
			auto _q = trans.get_dst().get_state();
			/// set up successors' stack contents
			auto _local(c.get_local());
			_local[i] = trans.get_dst().get_alpha();
			successors.emplace_back(_q, _local);
			if (trans.get_oper_type() == type_stack_operation::POP)
				generators[_q].emplace(_q, _local);
		}
	}
	return successors;
}

/**
 * Obtain the top of configuration
 * @param tau
 * @return
 */
top_config base_cuba::top_mapping(const explicit_config& tau) {
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
 * Print approximation
 */
void base_cuba::print_approximation(const vector<set<top_config>>& approx_R) {
	for (const auto& v : approx_R) {
		if (v.size() == 0)
			continue;
		cout << "  ";
		for (const auto& c : v)
			cout << c << " ";
		cout << "\n";
	}
}

} /* namespace cuba */

