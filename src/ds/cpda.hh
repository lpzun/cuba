/******************************************************************************
 * cpda.hh
 *
 * @date  : Aug 27, 2016
 * @author: TODO
 *
 * This file defines the data structures of a pushdown system (PDS), and a
 * concurrent pushdown system (CPDS):
 *   1. A PDS is a tuple (Q, L, D) consisting of three finite sets, called the
 * of control states Q, the stack alphabet L, and the pushdown program D,
 * respectively.
 *   2. A CPDS over n threads is a tuple (Q, L, D1, ..., Dn) such that, for
 * each i = 1..n, (Q, L, Di) is a PDS.
 ******************************************************************************/

#ifndef DS_CPDA_HH_
#define DS_CPDA_HH_

#include "fsa.hh"
#include "pda.hh"

namespace ruba {

/// the size of threads
using size_n = ushort;
/// the size of context switches
using size_k = ushort;
/// thread id
using id_thread_state = uint;

using id_thread = size_n;
using ctx_bound = size_k;

/**
 * concurrent pushdown automaton
 */
using concurrent_pushdown_automata = vector<pushdown_automaton>;

/// the set of stacks in CPDS
using stack_vec = vector<pda_stack>;

/**
 * Visible state
 */
class visible_state {
public:
	visible_state(const pda_state& s, const size_n& n);
	visible_state(const pda_state& s, const vector<pda_state>& L);
	~visible_state();

	const vector<pda_alpha>& get_local() const {
		return L;
	}

	pda_state get_state() const {
		return s;
	}

private:
	pda_state s;
	vector<pda_alpha> L;
};

/**
 * overloading operator << to print a visible state
 * @param os an output stream
 * @param s  a visible state
 * @return a reference to output stream
 */
inline ostream& operator<<(ostream& os, const visible_state& s) {
	os << "(" << s.get_state() << prop::SHARED_LOCAL_DELIMITER;
	if (s.get_local().size() > 0) {
		if (s.get_local()[0] == alphabet::EPSILON)
			os << alphabet::OPT_EPSILON;
		else
			os << s.get_local()[0];
	}
	for (uint i = 1; i < s.get_local().size(); ++i) {
		cout << ",";
		if (s.get_local()[i] == alphabet::EPSILON)
			os << alphabet::OPT_EPSILON;
		else
			os << s.get_local()[i];
	}
	cout << ")";
	return os;
}

/**
 * overloading operator <, return true if s1 < s2 and false otherwise
 * @param s1 a visible state
 * @param s2 a visible state
 * @return bool
 */
inline bool operator<(const visible_state& s1, const visible_state& s2) {
	if (s1.get_state() == s2.get_state()) {
		return algs::compare(s1.get_local(), s2.get_local()) == -1;
	}
	return s1.get_state() < s2.get_state();
}

/**
 * overloading operator >, return true if s1 > s2 and false otherwise
 * @param s1 a visible state
 * @param s2 a visible state
 * @return bool
 */
inline bool operator>(const visible_state& s1, const visible_state& s2) {
	return s2 < s1;
}

/**
 * overloading operator ==, return true if s1 == s2 and false otherwise
 * @param s1 a visible state
 * @param s2 a visible state
 * @return bool
 */
inline bool operator==(const visible_state& s1, const visible_state& s2) {
	if (s1.get_state() == s2.get_state()) {
		return algs::compare(s1.get_local(), s2.get_local()) == 0;
	}
	return false;
}

/**
 * overloading operator !=, return true if s1 != s2 and false otherwise
 * @param s1 a visible state
 * @param s2 a visible state
 * @return bool
 */
inline bool operator!=(const visible_state& s1, const visible_state& s2) {
	return !(s1 == s2);
}

/**
 * Explicit state of a CPDS is of the form (s|w1,...,wn). It is an element
 * of Qx(L*)^n, where n represents a number of threads
 */
class explicit_state {
public:
	explicit_state(const pda_state& s, const size_n& n);
	explicit_state(const pda_state& s, const stack_vec& W);
	explicit_state(const explicit_state& c);
	~explicit_state();

	pda_state get_state() const {
		return s;
	}

	const stack_vec& get_stacks() const {
		return W;
	}

	visible_state top();
	visible_state top() const;

private:
	pda_state s;
	stack_vec W;
};

/**
 * overloading operator <<: print a configuration of the CPDS
 * @param os an output
 * @param c
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const explicit_state& c) {
	os << "(" << c.get_state() << prop::SHARED_LOCAL_DELIMITER;
	if (c.get_stacks().size() > 0)
		os << c.get_stacks()[0];
	for (uint i = 1; i < c.get_stacks().size(); ++i)
		os << prop::THREAD_DELIMITER << c.get_stacks()[i];
	os << ")";
	return os;
}

/**
 * overloading the operator <
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator<(const explicit_state& g1, const explicit_state& g2) {
	return g1.top() < g2.top();
}

/**
 * overloading the operator >
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator>(const explicit_state& g1, const explicit_state& g2) {
	return g2 < g1;
}

/**
 * overloading the operator ==
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator==(const explicit_state& g1, const explicit_state& g2) {
	if (g1.get_state() == g2.get_state()) {
		auto iw1 = g1.get_stacks().cbegin();
		auto iw2 = g2.get_stacks().cbegin();
		while (iw1 != g1.get_stacks().cend()) {
			if (*iw1 != *iw2)
				return false;
			++iw1, ++iw2;
		}
		return true;
	}
	return false;
}

/**
 * overloading the operator !=
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator!=(const explicit_state& g1, const explicit_state& g2) {
	return !(g1 == g2);
}

/**
 * Explicit state -- with thread id and contexts -- of a CPDS is of the form
 * (id,k,s|w1,...,wn). It is an element of Q x (L*)^n, where n represents a
 * number of threads
 */
class explicit_state_tid: public explicit_state {
public:
	explicit_state_tid(const pda_state& s, const size_n& n);
	explicit_state_tid(const id_thread& id, const ctx_bound& k,
			const pda_state& s, const size_n& n);
	explicit_state_tid(const id_thread& id, const pda_state& s,
			const stack_vec& W);
	explicit_state_tid(const id_thread& id, const ctx_bound& k,
			const pda_state& s, const stack_vec& W);
	explicit_state_tid(const explicit_state_tid& c);
	~explicit_state_tid();

	ctx_bound get_context_k() const {
		return k;
	}

	void set_context_k(const ctx_bound& k) {
		this->k = k;
	}

	id_thread get_thread_id() const {
		return id;
	}

private:
	/// to mark the active thread that reach current state
	id_thread id;
	/// the number of contexts used to reach current state
	ctx_bound k;
};

/**
 * overloading operator <<: print a configuration of the CPDS
 * @param os an output stream
 * @param c  an explicit_state_id
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const explicit_state_tid& c) {
	if (c.get_thread_id() == c.get_stacks().size())
		os << "t=" << "* ";
	else
		os << "t=" << c.get_thread_id() << " ";
	os << "<" << c.get_state() << prop::SHARED_LOCAL_DELIMITER;
	if (c.get_stacks().size() > 0)
		os << c.get_stacks()[0];
	for (uint i = 1; i < c.get_stacks().size(); ++i)
		os << prop::THREAD_DELIMITER << c.get_stacks()[i];
	os << ">";
	return os;
}

/**
 * overloading the operator <, return true if g1 < g2 and false otherwise
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator<(const explicit_state_tid& g1,
		const explicit_state_tid& g2) {
	return g1.top() < g2.top();
}

/**
 * overloading the operator >, return true if g1 > g2 and false otherwise
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator>(const explicit_state_tid& g1,
		const explicit_state_tid& g2) {
	return g2 < g1;
}

/**
 * overloading the operator ==, return true if g1 == g2 and false otherwise
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator==(const explicit_state_tid& g1,
		const explicit_state_tid& g2) {
	if (g1.get_state() == g2.get_state()) {
		auto iw1 = g1.get_stacks().cbegin();
		auto iw2 = g2.get_stacks().cbegin();
		while (iw1 != g1.get_stacks().cend()) {
			if (*iw1 != *iw2)
				return false;
			++iw1, ++iw2;
		}
		return true;
	}
	return false;
}

/**
 * overloading the operator !=, return true if g1 != g2 and false otherwise
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator!=(const explicit_state_tid& g1,
		const explicit_state_tid& g2) {
	return !(g1 == g2);
}


/**
 *
 * Define a pushdown store automaton. A pushdown store automaton is a
 * finite automaton, with the following specific features:
 *  - states: this contains only intermediate states and accept states,
 *    no start states;
 *  - start: this contains only start states;
 *  - accept: the accept state is a state. Its id is defined between the
 *    IDs of start states and those of intermediate states, e.g., initial
 *    0..5, accept 6, states 7..9
 *
 *  - a static variable interm_s: to generate an intermediate state
 */
class store_automaton: public finite_automaton {
public:
	store_automaton(const fsa_state_set& states, const fsa_alphabet& alphabet,
			const fsa_delta& transitions, const fsa_state_set& start,
			const fsa_state& accept);
	~store_automaton();

	static fsa_state create_interm_state();
private:
	static fsa_state interm_s;
};

/**
 * Symbolic state of a CPDS is of the form (s|A1,...,An), where Ai is a
 * pushdown store automaton -- a symbolic representation -- of reachable
 * thread states.
 */
class symbolic_state {
public:
	symbolic_state(const pda_state& q, const vector<store_automaton>& W);
	symbolic_state(const pda_state& q, const size_n&n,
			const store_automaton& A);
	~symbolic_state();

	pda_state get_state() const {
		return q;
	}

	const vector<store_automaton>& get_automata() const {
		return W;
	}

private:
	/// a share state or control state
	pda_state q;
	/// a vector of pushdown store automaton
	vector<store_automaton> W;
};

/**
 * overloading operator << to print a symbolic state
 * @param os an output stream
 * @param c  a symbolic state
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const symbolic_state& c) {
	os << "<" << c.get_state() << "|";
	if (c.get_automata().size() > 0) {
		for (uint i = 0; i < c.get_automata().size() - 1; ++i) {
			os << "A" << i << ",";
		}
		os << "A" << c.get_automata().size() - 1 << ">\n";
		for (uint i = 0; i < c.get_automata().size(); ++i) {

			os << "A" << i << ": " << c.get_automata()[i] << "\n";
		}
	} else {
		os << ">" << endl;
	}
	return os;
}
/// A finite machine: used for overapproximate reachable visible states.
/// We definite a finite state machine in the form of adjacency list.
using finite_machine = map<thread_visible_state, deque<transition<thread_visible_state, thread_visible_state>>>;

/// Concurrent finite machine: a vector of finite state machine
using concurrent_finite_machine = vector<finite_machine>;

/**
 * The result data structure of observation sequences
 */
enum class sequence {
	CONVERGENT, DIVERGENT, REACHABLE, UNKNOWN
};

}
/* namespace ruba */

#endif /* DS_CPDA_HH_ */
