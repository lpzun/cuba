/**
 * state.cc
 *
 * @date  : Aug 27, 2016
 * @author: Peizun Liu
 */

#include "state.hh"

namespace cuba {

/**
 * A default constructor without any input parameters
 */
thread_config::thread_config() :
        s(), w() {
}
/**
 * A constructor with a control state and an alphabet
 * @param s
 * @param w
 */
thread_config::thread_config(const control_state& s, const alphabet& w) :
        s(s), w(w) {
}
/**
 * A constructor with a thread configuration
 * @param c
 */
thread_config::thread_config(const thread_config& c) :
        s(c.get_state()), w(c.get_stack()) {
}

thread_config::~thread_config() {
}

} /* namespace bssp */
