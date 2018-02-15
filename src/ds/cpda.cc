/**
 * state.cc
 *
 * @date  : Aug 27, 2016
 * @author: TODO
 */

#include "cpda.hh"

namespace ruba {

pda_state thread_visible_state::S = 0;

/**
 * default constructor
 */
thread_visible_state::thread_visible_state() :
		s(0), l(0) {
}

/**
 * constructor with a control state and a stack symbol
 * @param s
 * @param l
 */
thread_visible_state::thread_visible_state(const pda_state& s,
		const pda_alpha& l) :
		s(s), l(l) {
}

/**
 * destructor
 */
thread_visible_state::~thread_visible_state() {
}

/**
 * A default constructor without any input parameters
 */
thread_state::thread_state() :
		s(), w() {
}

/**
 * A constructor with a control state and a stack symbol
 * @param s
 * @param l
 */
thread_state::thread_state(const pda_state& s, const pda_alpha& l) :
		s(s), w() {
	w.push(l);
}

/**
 * A constructor with a thread state
 * @param t
 */
thread_state::thread_state(const thread_visible_state& t) :
		s(t.get_state()), w() {
	w.push(t.get_alpha());
}
/**
 * A constructor with a control state and an alphabet
 * @param s
 * @param w
 */
thread_state::thread_state(const pda_state& s, const pda_stack& w) :
		s(s), w(w) {
}

/**
 * A constructor with a thread configuration
 * @param c
 */
thread_state::thread_state(const thread_state& c) :
		s(c.get_state()), w(c.get_stack()) {
}

/**
 * destructor
 */
thread_state::~thread_state() {

}

/**
 * A constructor with a control state and the size of local part
 * @param s
 * @param n
 */
visible_state::visible_state(const pda_state& s, const size_n& n) :
		s(s), L(n) {

}

/**
 * A constructor with a control state and a vector of local states
 * @param s
 * @param L
 */
visible_state::visible_state(const pda_state& s, const vector<pda_alpha>& L) :
		s(s), L(L) {

}

/**
 * destructor
 */
visible_state::~visible_state() {

}

/**
 * A constructor with a control state and the number of concurrent
 * components
 * @param s
 * @param n
 */
explicit_state::explicit_state(const pda_state& s, const size_n& n) :
		s(s), W(n) {
}

/**
 * A constructor with a thread id,a  context bound k, a control state
 * and the number of concurrent
 * @param id
 * @param k
 * @param s
 * @param W
 */
explicit_state::explicit_state(const pda_state& s, const stack_vec& W) :
		s(s), W(W) {

}

/**
 * A constructor with a global configuration
 * @param g
 */
explicit_state::explicit_state(const explicit_state& c) :
		s(c.get_state()), W(c.get_stacks()) {
}

/**
 * destructor
 */
explicit_state::~explicit_state() {

}

/**
 * return a top configuration
 * @return a global state
 */
visible_state explicit_state::top() const {
	vector<pda_alpha> L(W.size());
	for (uint i = 0; i < W.size(); ++i) {
		L[i] = W[i].top();
	}
	return visible_state(s, L);
}

/**
 * return a top configuration
 * @return a global state
 */
visible_state explicit_state::top() {
	vector<pda_alpha> L(W.size());
	for (uint i = 0; i < W.size(); ++i) {
		L[i] = W[i].top();
	}
	return visible_state(s, L);
}

/**
 *
 * @param s
 * @param n
 */
explicit_state_tid::explicit_state_tid(const pda_state& s, const size_n& n) :
		explicit_state(s, n), id(0), k(0) {

}

/**
 *
 * @param id
 * @param k
 * @param s
 * @param n
 */
explicit_state_tid::explicit_state_tid(const id_thread& id,
		const ctx_bound& k, const pda_state& s, const size_n& n) :
		explicit_state(s, n), id(id), k(k) {

}

/**
 * Constructor with thread id, PDA state and the vector of stacks
 * @param id
 * @param s
 * @param W
 */
explicit_state_tid::explicit_state_tid(const id_thread& id,
		const pda_state& s, const stack_vec& W) :
		explicit_state(s, W), id(id), k(0) {

}

/**
 *
 * @param id
 * @param k
 * @param s
 * @param W
 */
explicit_state_tid::explicit_state_tid(const id_thread& id,
		const ctx_bound& k, const pda_state& s, const stack_vec& W) :
		explicit_state(s, W), id(id), k(k) {

}

explicit_state_tid::explicit_state_tid(const explicit_state_tid& c) :
		explicit_state(c.get_state(), c.get_stacks()), id(c.get_thread_id()), k(
				c.get_context_k()) {

}

explicit_state_tid::~explicit_state_tid() {

}
/////////////////////////////////////////////////////////////////////////
/// The data structure for pushdown store automaton
///
/////////////////////////////////////////////////////////////////////////
/**
 *
 * @param states
 * @param alphabet
 * @param transitions
 * @param start
 * @param accept
 */
store_automaton::store_automaton(const fsa_state_set& states,
		const fsa_alphabet& alphabet, const fsa_delta& transitions,
		const fsa_state_set& start, const fsa_state& accept) :
		finite_automaton(states, alphabet, transitions, start, accept) {
}

/**
 * destructor
 */
store_automaton::~store_automaton() {

}

fsa_state store_automaton::interm_s = 0;

/**
 *
 * @return
 */
fsa_state store_automaton::create_interm_state() {
	if (interm_s == 0)
		interm_s = thread_visible_state::S;
	if (interm_s == std::numeric_limits<uint>::max())
		throw cuba_runtime_error("intermediate state: overflow!");
	return interm_s++;
}

/////////////////////////////////////////////////////////////////////////
/// PART 3. The data structure for aggregate configuration
///
/////////////////////////////////////////////////////////////////////////

/**
 * Constructor with control state g and a list of finite automaton W
 * @param g
 * @param W
 */
symbolic_state::symbolic_state(const pda_state& g,
		const vector<store_automaton>& W) :
		q(g), W(W) {
}

/**
 * constructor control state g, the number of finite automation, and a
 * finite automaton
 * @param g
 * @param n
 * @param A
 */
symbolic_state::symbolic_state(const pda_state& g, const size_n&n,
		const store_automaton& A) :
		q(g), W(n, A) {
}

/**
 * destructor
 */
symbolic_state::~symbolic_state() {

}
} /* namespace ruba */
