/**
 * generator.cc
 *
 * @date  : Jan 19, 2018
 * @author: Peizun Liu
 */

#include <generator.hh>

namespace ruba {

/////////////////////////////////////////////////////////////////////////
/// PART 2. A preprocessor, overapproximate the set of reachable top
/// configurations
///
/////////////////////////////////////////////////////////////////////////
/**
 * Default constructor
 * @param initl   : initial configuration
 * @param filename: input CPDS
 */
generator::generator(const string& initl, const string& filename) :
		generators(vector<set<top_config>>(thread_visible_state::S)) {
	context_insensitive(initl, filename);
}

generator::~generator() {

}

/**
 *
 * @param c_I : an initial top configuration
 * @param cfsm: concurrent finite machine
 */
void generator::context_insensitive(const string& initl,
		const string& filename) {
	const auto& initl_c = parser::parse_input_cfg(initl);
	context_insensitive(top_mapping(initl_c), filename);
}

/**
 * A constructor with initial top configuration and a concurrent finite machine
 *
 * @param initl   : initial configuration
 * @param filename: the file name for CPDS
 */
void generator::context_insensitive(const top_config& initl,
		const string& filename) {
	const auto& CFSM = parser::parse_input_cfsm(filename);
	const auto& approx_Z = context_insensitive(initl, CFSM);

	if (prop::OPT_PRINT_ALL) {
		cout << "Approximation Z:\n";
		this->print_approximation(approx_Z);
		cout << "G intersects Z:\n";
		this->print_approximation(generators);

		/// Print Concurrent FSM
		for (const auto& fsm : CFSM) {
			for (const auto& adjlist : fsm)
				for (const auto& r : adjlist.second)
					cout << r << "\n";
		}
	}
}

/**
 *
 * @param top_initl
 * @param CFSM
 * @return
 */
vector<set<top_config>> generator::context_insensitive(
		const top_config& top_initl, const vector<finite_machine>& CFSM) {
	return standard_FWS(top_initl, CFSM);
}

/**
 * Standard forward search: to compute the overapproximation of the set of
 * reachable top configurations.
 *
 * @return a set of reachable top configurations
 */
vector<set<top_config>> generator::standard_FWS(const top_config& initl_c,
		const vector<finite_machine>& CFSM) {
	/// step 1: set up the data structures
	/// 1.1 <approx_Z>: the overapproximation of the set of reachable
	///     top configurations
	vector<set<top_config>> approx_Z(thread_visible_state::S);
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
deque<top_config> generator::step(const top_config& c,
		const vector<finite_machine>& CFSM) {
	deque<top_config> successors;
	for (uint i = 0; i < c.get_local().size(); ++i) {
		auto ifind = CFSM[i].find(
				thread_visible_state(c.get_state(), c.get_local()[i]));
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
top_config generator::top_mapping(const explicit_state& tau) {
	vector<pda_alpha> W(tau.get_stacks().size());
	for (size_n i = 0; i < tau.get_stacks().size(); ++i) {
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
void generator::print_approximation(const vector<set<top_config>>& approx_R) {
	for (const auto& v : approx_R) {
		if (v.size() == 0)
			continue;
		cout << "  ";
		for (const auto& c : v)
			cout << c << " ";
		cout << "\n";
	}
}

} /* namespace ruba */
