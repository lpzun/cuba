/**
 * state.cc
 *
 * @date  : Aug 27, 2016
 * @author: Peizun Liu
 */

#include "cpda.hh"

namespace cuba {

pda_state thread_state::S = 0;
pda_alpha thread_state::L = 0;

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
thread_state::thread_state(const pda_state& s, const pda_alpha& l) :
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
thread_config::thread_config(const pda_state& s, const pda_alpha& l) :
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
thread_config::thread_config(const pda_state& s, const pda_stack& w) :
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
global_state::global_state(const pda_state& s, const size_t& n) :
		s(s), L(n) {

}

/**
 * A constructor with a control state and a vector of local states
 * @param s
 * @param L
 */
global_state::global_state(const pda_state& s, const vector<pda_alpha>& L) :
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
concrete_config::concrete_config(const pda_state& s, const size_t& n) :
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
concrete_config::concrete_config(const id_thread& id, const ctx_bound& k,
		const pda_state& s, const size_t& n) :
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
concrete_config::concrete_config(const id_thread& id, const ctx_bound& k,
		const pda_state& s, const stack_vec& W) :
		id(id), k(k), s(s), W(W) {

}

/**
 * A constructor with a global configuration
 * @param g
 */
concrete_config::concrete_config(const concrete_config& g) :
		id(g.get_thread_id()), k(g.get_context_k()), s(g.get_state()), ///
		W(g.get_stacks()) {

}

/**
 * destructor
 */
concrete_config::~concrete_config() {

}

/**
 * return a top configuration
 * @return a global state
 */
global_state concrete_config::top() const {
	vector<pda_alpha> L(W.size());
	for (auto i = 0; i < W.size(); ++i) {
		L[i] = W[i].top();
	}
	return global_state(s, L);
}

/**
 * return a top configuration
 * @return a global state
 */
global_state concrete_config::top() {
	vector<pda_alpha> L(W.size());
	for (auto i = 0; i < W.size(); ++i) {
		L[i] = W[i].top();
	}
	return global_state(s, L);
}

/////////////////////////////////////////////////////////////////////////
/// PART 2. The data structure for aggregate configuration
///
/////////////////////////////////////////////////////////////////////////

/**
 * constructor with control state g and a list of finite automaton W
 * @param g
 * @param W
 */
symbolic_config::symbolic_config(const pda_state& g,
		const vector<finite_automaton>& W) :
		g(g), W(W) {
}

/**
 * constructor control state g, the number of finite automation, and a
 * finite automaton
 * @param g
 * @param n
 * @param A
 */
symbolic_config::symbolic_config(const pda_state& g, const size_t&n,
		const finite_automaton& A) :
		g(g), W(n, A) {
}

/**
 * destructor
 */
symbolic_config::~symbolic_config() {

}
} /* namespace bssp */
