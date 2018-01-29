/**
 * The implementation of base_cuba class
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
		const string& filename, const size_n n) :
		initl_c(0, 1), final_c(0, 1), CPDA(), generators(), reachable_T() {
	if (prop::OPT_PARAMETERIZED) {

	} else {
		initl_c = parser::parse_input_cfg(initl);
		final_c = parser::parse_input_cfg(final);
		CPDA = parser::parse_input_cpds(filename);
	}

	/// set up overapproximation of reachable top configurations
	generator gen(initl, filename);
	generators = gen.get_generators();
}

base_cuba::~base_cuba() {

}
} /* namespace cuba */

