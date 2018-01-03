/**
 * Write-(un)bounded analysis
 *
 * @date  : Jan 1, 2018
 * @author: Peizun Liu
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
		const string& filename, const size_n n) :
		n_bound(n), initl_c(0, 1), final_c(0, 1), CPDA(), ///
		generators(), reachable_T(), ///
		convergence_GS(0), convergence_VS(0) {

	initl_c = parser::parse_input_cfg(initl);
	final_c = parser::parse_input_cfg(final);
	CPDA = parser::parse_input_cpds(filename);

	/// set up overapproximation of reachable top configurations
	generator gen(initl, filename);
	generators = gen.get_generators();
}

base_wuba::~base_wuba() {
}

} /* namespace wuba */
