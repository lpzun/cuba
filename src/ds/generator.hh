/**
 * generator.hh
 *
 * @date  : Jan 19, 2018
 * @author: TODO
 */

#ifndef DS_GENERATOR_HH_
#define DS_GENERATOR_HH_

#include "parsers.hh"

namespace ruba {

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following are for the generator idea
///
/////////////////////////////////////////////////////////////////////////
class generator {
public:
	generator(const string& initl, const concurrent_pushdown_automata& CPDA,
			const concurrent_finite_machine& CFSM);
	~generator();

	vector<set<visible_state>> get_generators() const {
		return generators_for_dynamic_bound;
	}

private:
	vector<set<visible_state>> generators_for_fixed_bound_1;
	/// 1.1 <approx_Z>: the overapproximation of the set of reachable
	///     top configurations
	vector<set<visible_state>> approx_Z;
	vector<set<visible_state>> generators_for_dynamic_bound;

	const explicit_state initl_tau;
	concurrent_pushdown_automata CPDA;
	concurrent_finite_machine CFSM;

	void context_insensitive();

	void context_insensitive_with_dynamic_bound();
	void dynamic_bound_standard_FWS();
	deque<explicit_state> step(const explicit_state& c);
	void step(const pda_state& q, const stack_vec& W, const uint tid,
			deque<explicit_state>& successors);
	bool update_R(const explicit_state& tau, vector<deque<explicit_state>>& R);

	/// Build FSM and then compute Z
	void context_insensitive_with_fixed_bound_1();
	vector<set<visible_state>> fixed_bound_standard_FWS();
	deque<visible_state> step(const visible_state& c);

	/// Utilities to compute & print visible states
	visible_state top_mapping(const explicit_state& tau);
	void print_approximation(const vector<set<visible_state>>& approx_R) const;
};

} /* namespace ruba */

#endif /* DS_GENERATOR_HH_ */
