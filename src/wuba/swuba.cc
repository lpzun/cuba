/**
 * Symbolic write-unbounded analysis
 *
 * @date  : Jan 2, 2018
 * @author: Peizun Liu
 */

#include "wuba.hh"

namespace wuba {
/**
 * Constructor
 * @param initl
 * @param final
 * @param filename
 * @param n_bound
 */
symbolic_wuba::symbolic_wuba(const string& initl, const string& final,
		const string& filename, const size_n n_bound) :
		base_wuba(initl, final, filename, n_bound) {

}

/**
 * Destructor
 */
symbolic_wuba::~symbolic_wuba() {

}

void symbolic_wuba::write_bounded_analysis(const size_k k_bound) {
	cout << "symbolic WUBA is not set up yet\n";
}

void symbolic_wuba::write_unbounded_analysis() {

}

}
/* namespace wuba */
