/**
 * state.cc
 *
 * @date  : Aug 27, 2016
 * @author: Peizun Liu
 */

#include "cpda.hh"

namespace cuba {

control_state thread_state::S = 0;
stack_symbol thread_state::L = 0;

/**
 * default constructor
 */
thread_state::thread_state() :
		s(0), l(0) {
}

/**
 * constructor with a control state and a stack symbol
 * @param s
 * @param l
 */
thread_state::thread_state(const control_state& s, const stack_symbol& l) :
		s(s), l(l) {
}

/**
 * destructor
 */
thread_state::~thread_state() {
}

/**
 * A default constructor without any input parameters
 */
thread_config::thread_config() :
		s(), w() {
}

/**
 * A constructor with a control state and a stack symbol
 * @param s
 * @param l
 */
thread_config::thread_config(const control_state& s, const stack_symbol& l) :
		s(s), w() {
	w.push(l);
}

/**
 * A constructor with a thread state
 * @param t
 */
thread_config::thread_config(const thread_state& t) :
		s(t.get_state()), w() {
	w.push(t.get_symbol());
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
 * A constructor with a control state and the size of local part
 * @param s
 * @param n
 */
global_state::global_state(const control_state& s, const size_t& n) :
		s(s), L(n) {

}

/**
 * A constructor with a control state and a vector of local states
 * @param s
 * @param L
 */
global_state::global_state(const control_state& s,
		const vector<stack_symbol>& L) :
		s(s), L(L) {

}

/**
 * destructor
 */
global_state::~global_state() {

}

/**
 * A constructor with a control state and the number of concurrent
 * components
 * @param s
 * @param n
 */
global_config::global_config(const control_state& s, const size_t& n) :
		id(0), k(0), s(s), W(n) {
}

/**
 * A constructor with a thread id,a  context bound k, a control state
 * and the number of concurrent
 * @param id
 * @param k
 * @param s
 * @param n
 */
global_config::global_config(const id_thread& id, const ctx_bound& k,
		const control_state& s, const size_t& n) :
		id(id), k(k), s(s), W(n) {

}

/**
 * A constructor with a thread id,a  context bound k, a control state
 * and the number of concurrent
 * @param id
 * @param k
 * @param s
 * @param W
 */
global_config::global_config(const id_thread& id, const ctx_bound& k,
		const control_state& s, const stack_vec& W) :
		id(id), k(k), s(s), W(W) {

}

/**
 * A constructor with a global configuration
 * @param g
 */
global_config::global_config(const global_config& g) :
		id(g.get_thread_id()), k(g.get_context_k()), s(g.get_state()), ///
		W(g.get_stacks()) {

}

/**
 * destructor
 */
global_config::~global_config() {

}

/**
 * return a top configuration
 * @return a global state
 */
global_state global_config::top() const {
	vector<stack_symbol> L(W.size());
	for (auto i = 0; i < W.size(); ++i) {
		L[i] = W[i].top();
	}
	return global_state(s, L);
}

/**
 * return a top configuration
 * @return a global state
 */
global_state global_config::top() {
	vector<stack_symbol> L(W.size());
	for (auto i = 0; i < W.size(); ++i) {
		L[i] = W[i].top();
	}
	return global_state(s, L);
}

} /* namespace bssp */
