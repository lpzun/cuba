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

finite_automaton::finite_automaton() {

}

finite_automaton::~finite_automaton() {

}

} /* namespace cuba */
