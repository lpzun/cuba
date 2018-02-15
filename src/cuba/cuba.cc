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
 * @param final final global state
 * @param filename input CPDs
 */
base_cuba::base_cuba(const string& initl, const string& final,
		const string& filename) :
		initl_c(0, 1), final_c(0, 1), CPDA(), generators(), reachable_T() {
	initl_c = parser::parse_input_cfg(initl);
	final_c = parser::parse_input_cfg(final);
	CPDA = parser::parse_input_cpds(filename);

	generator gen(initl, filename);
	generators = gen.get_generators();
}

base_cuba::~base_cuba() {

}
} /* namespace cuba */

