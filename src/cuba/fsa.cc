/**
 * @brief fsa.cc
 *
 * @date  : Sep 15, 2016
 * @author: lpzun
 */

#include "fsa.hh"

namespace cuba {

/**
 * default constructor
 */
fsa_transition::fsa_transition() :
		src(0), dst(0), label(0) {

}

/**
 * constructor with source state, destination state and label, a transition
 * likes as follows:
 *     (src, label, dst)
 * @param src
 * @param dst
 * @param label
 */
fsa_transition::fsa_transition(const fsa_state& src, const fsa_state& dst,
		const fsa_alpha& label) :
		src(src), dst(dst), label(label) {

}

/**
 * default destructor of fsa transition
 */
fsa_transition::~fsa_transition() {

}

/**
 * constructor with the biggest state, the biggest input symbol (alpha),
 * the set of transitions and the accept state
 * @param state
 * @param alpha
 * @param transs
 * @param accept
 */
finite_automaton::finite_automaton(const fsa_state& state,
		const fsa_alpha& alpha, const fsa_delta& transs,
		const fsa_state& accept) :
		states(state), alphabet(alpha), transitions(transs), accept_state(
				accept) {

}

/**
 * constructor with the biggest state, the biggest input symbol (alpha),
 * and the accept state
 * @param state
 * @param alpha
 * @param accept
 */
finite_automaton::finite_automaton(const fsa_state& state,
		const fsa_alpha& alpha, const fsa_state& accept) :
		states(state), alphabet(alpha), transitions(), accept_state(accept) {

}

/**
 * destructor
 */
finite_automaton::~finite_automaton() {

}

} /* namespace cuba */
