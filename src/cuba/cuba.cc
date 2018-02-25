/**
 * base_cuba
 *    The implementation of base_cuba class
 *
 * @date   Jun 13, 2017
 * @author TODO
 */
#include "cuba.hh"

namespace cuba {
/**
 * Constructor
 *   Set up the initial global state, target state, concurrent pushdown
 *   system and the overapproximation of reachable top configurations.
 *
 * @param initl initial global state
 * @param final target visible state
 * @param filename input CPDs
 */
base_cuba::base_cuba(const string& initl, const string& final,
		const string& filename) :
		reachable(false), initl_c(0, 1), final_c(0, 1), CPDA(), generators(), reachable_T() {
	CPDA = parser::parse_input_cpds(filename);

	string initl_s(initl);
	if (initl_s.find("...") != std::string::npos)
		initl_s = parser::create_default_states_in_str(CPDA.size());

	initl_c = parser::parse_input_cfg(initl_s);
	if (prop::OPT_PROB_REACHABILITY)
		final_c = top_mapping(parser::parse_input_cfg(final));

	generator gen(initl_s, filename);
	generators = gen.get_generators();
}

base_cuba::~base_cuba() {

}

/**
 * Extract tau's visible state
 * @param tau
 * @return visible_state
 */
visible_state base_cuba::top_mapping(const explicit_state& tau) {
	vector<pda_alpha> W(tau.get_stacks().size());
	for (size_n i = 0; i < tau.get_stacks().size(); ++i) {
		if (tau.get_stacks()[i].empty())
			W[i] = alphabet::EPSILON;
		else
			W[i] = tau.get_stacks()[i].top();
	}
	return visible_state(tau.get_state(), W);
}
} /* namespace cuba */

