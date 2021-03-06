/**
 * Write-(un)bounded analysis
 *
 * @date   Jan 1, 2018
 * @author TODO
 */

#include "wuba.hh"

namespace wuba {

/**
 *
 * @param initl
 * @param final
 * @param filename
 * @param n
 */
base_wuba::base_wuba(const string& initl, const string& final,
		const string& filename) :
		initl_c(0, 1), final_c(0, 1), CPDA(), ///
		generators(), reachable_T(), ///
		convergence_GS(0), convergence_VS(0) {
	cout << "write-(un)bounded analysis......\n";

	initl_c = parser::parse_input_cfg(initl);
	final_c = parser::parse_input_cfg(final);
	CPDA = parser::parse_input_cpds(filename);

	/// set up overapproximation of reachable top configurations
	generator gen(initl, CPDA, parser::parse_input_cfsm(filename));
	generators = gen.get_generators();
}

base_wuba::~base_wuba() {
}

} /* namespace wuba */
