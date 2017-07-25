/**
 * @brief pds.cc
 *
 * @date  : Sep 15, 2016
 * @author: lpzun
 */

#include "pda.hh"

namespace cuba {

const pda_alpha alphabet::EPSILON = -1;
const pda_alpha alphabet::NULLPTR = -2;
const char alphabet::OPT_EPSILON = '-';

/**
 * constructor with default parameters
 */
pushdown_automaton::pushdown_automaton() :
		states(), alphas(), actions(), program() {

}

/**
 * constructor
 *
 * @param states
 * @param alphas
 * @param actions
 * @param PDA
 */
pushdown_automaton::pushdown_automaton(const set<pda_state>& states,
		const set<pda_alpha>& alphas, const vector<pda_action>& actions,
		const adj_list& PDA) :
		states(states), alphas(alphas), actions(actions), program(PDA) {
}

/**
 * destructor
 */
pushdown_automaton::~pushdown_automaton() {
}

} /* namespace cuba */
