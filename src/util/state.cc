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
thread_config::thread_config(const control_state& s, const sstack& w) :
        s(s), w(w) {
}

/**
 * A constructor with a thread configuration
 * @param c
 */
thread_config::thread_config(const thread_config& c) :
        s(c.get_state()), w(c.get_stack()) {
}

/**
 * destructor
 */
thread_config::~thread_config() {

}

/**
 * A constructor with a control state and the number of concurrent
 * components
 * @param s
 * @param n
 */
global_config::global_config(const control_state& s, const size_t& n) :
        s(s), W(n) {

}

/**
 * A constructor with a control state and concurrent components
 * @param s
 * @param W
 */
global_config::global_config(const control_state& s, const cstack& W) :
        s(s), W(W) {

}

/**
 * destructor
 */
global_config::~global_config() {

}

} /* namespace bssp */
