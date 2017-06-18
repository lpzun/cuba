/******************************************************************************
 * @brief cpda.hh
 *
 * @date  : Aug 27, 2016
 * @author: Peizun Liu
 *
 * This file defines the data structures of a pushdown system (PDS), and a
 * concurrent pushdown system (CPDS):
 *   1. A PDS is a tuple (Q, L, D) consisting of three finite sets, called the
 * of control states Q, the stack alphabet L, and the pushdown program D,
 * respectively.
 *   2. A CPDS over n threads is a tuple (Q, L, D1, ..., Dn) such that, for
 * each i, (Q, L, Di) is a PDS.
 ******************************************************************************/

#ifndef CUBA_CPDA_HH_
#define CUBA_CPDA_HH_

#include "pda.hh"
#include "fsa.hh"

namespace cuba {

/// the size of threads
using size_t = ushort;
/// the size of context switches
using size_k = ushort;

using id_thread_state = uint;

using id_thread = size_t;
using ctx_bound = size_k;

/////////////////////////////////////////////////////////////////////////
/// PART 1. global state and concrete configuration definitions are from
/// here.
///
/// global state:
/// concrete configuration: a concrete configuration of PDA
///
/////////////////////////////////////////////////////////////////////////

/**
 * concurrent pushdown automaton
 */
using concurrent_pushdown_automata = vector<pushdown_automaton>;

/////////////////////////////////////////////////////////////////////////
/// PART 1. global state and concrete configuration definitions are from
/// here.
///
/// global state:
/// concrete configuration: a concrete configuration of PDA
///
/////////////////////////////////////////////////////////////////////////
/// the set of stacks in CPDS
using stack_vec = vector<pda_stack>;

class global_state {
public:
	global_state(const pda_state& s, const size_t& n);
	global_state(const pda_state& s, const vector<pda_state>& L);
	~global_state();

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
 * overloading operator <<: print a global state
 * @param os
 * @param c
 * @return bool
 */
inline ostream& operator<<(ostream& os, const global_state& g) {
	os << "(" << g.get_state() << prop::SHARED_LOCAL_DELIMITER;
	if (g.get_local().size() > 0) {
		if (g.get_local()[0] == alphabet::EPSILON)
			os << alphabet::OPT_EPSILON;
		else
			os << g.get_local()[0];
	}
	for (uint i = 1; i < g.get_local().size(); ++i) {
		cout << ",";
		if (g.get_local()[i] == alphabet::EPSILON)
			os << alphabet::OPT_EPSILON;
		else
			os << g.get_local()[i];
	}
	cout << ")";
	return os;
}

/**
 * overloading operator <
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator<(const global_state& g1, const global_state& g2) {
	if (g1.get_state() == g2.get_state()) {
		return algs::compare(g1.get_local(), g2.get_local()) == -1;
	}
	return g1.get_state() < g2.get_state();
}

/**
 * overloading operator >
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator>(const global_state& g1, const global_state& g2) {
	return g2 < g1;
}

/**
 * overloading operator ==
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator==(const global_state& g1, const global_state& g2) {
	if (g1.get_state() == g2.get_state()) {
		return algs::compare(g1.get_local(), g2.get_local()) == 0;
	}
	return false;
}

/**
 * overloading operator !=
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator!=(const global_state& g1, const global_state& g2) {
	return !(g1 == g2);
}

/**
 * A configuration (s|w1,...,wn) of a CPDS is an element of Qx(L*)^n
 */
class concrete_config {
public:
	concrete_config(const pda_state& s, const size_t& n);
	concrete_config(const pda_state& s, const stack_vec& W);
	concrete_config(const concrete_config& c);
	~concrete_config();

	pda_state get_state() const {
		return s;
	}

	const stack_vec& get_stacks() const {
		return W;
	}

	global_state top();
	global_state top() const;

private:
	pda_state s;
	stack_vec W;
};

/**
 * overloading operator <<: print a configuration of the CPDS
 * @param os
 * @param c
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const concrete_config& c) {
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
inline bool operator<(const concrete_config& g1, const concrete_config& g2) {
	return g1.top() < g2.top();
}

/**
 * overloading the operator >
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator>(const concrete_config& g1, const concrete_config& g2) {
	return g2 < g1;
}

/**
 * overloading the operator ==
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator==(const concrete_config& g1, const concrete_config& g2) {
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
inline bool operator!=(const concrete_config& g1, const concrete_config& g2) {
	return !(g1 == g2);
}

/**
 *
 */
class global_config: public concrete_config {
public:
	global_config(const pda_state& s, const size_t& n);
	global_config(const id_thread& id, const ctx_bound& k, const pda_state& s,
			const size_t& n);
	global_config(const id_thread& id, const ctx_bound& k, const pda_state& s,
			const stack_vec& W);
	global_config(const global_config& c);
	~global_config();

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
	id_thread id; /// the active thread that reach current configuration
	ctx_bound k; /// the number of context switches used to reach current configuration
};

/**
 * overloading operator <<: print a configuration of the CPDS
 * @param os
 * @param c
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const global_config& c) {
	// os << "k=" << c.get_context_k() << " ";
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
 * overloading the operator <
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator<(const global_config& g1, const global_config& g2) {
	return g1.top() < g2.top();
}

/**
 * overloading the operator >
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator>(const global_config& g1, const global_config& g2) {
	return g2 < g1;
}

/**
 * overloading the operator ==
 * @param g1
 * @param g2
 * @return bool
 */
inline bool operator==(const global_config& g1, const global_config& g2) {
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
inline bool operator!=(const global_config& g1, const global_config& g2) {
	return !(g1 == g2);
}

/////////////////////////////////////////////////////////////////////////
/// PART 2. The data structure for symbolic configuration
///
/////////////////////////////////////////////////////////////////////////

/**
 * Define a pushdown store automaton. A pushdown store automaton is a
 * finite automaton, with the following specific features:
 *  - states: this contains only intermediate states and final states,
 *    no initial states;
 *  - initials: this contains only initial states;
 *  - accept: the accept state is a state defined between initial states
 *    and intermediate states, e.g., initial 0..5, accept 6, states 7..9
 */
using store_automaton = finite_automaton;

/**
 * Define a symbolic configuration
 */
class symbolic_config {
public:
	symbolic_config(const pda_state& q, const vector<store_automaton>& W);
	symbolic_config(const pda_state& q, const size_t&n,
			const store_automaton& A);
	~symbolic_config();

	pda_state get_state() const {
		return q;
	}

	const vector<store_automaton>& get_automata() const {
		return W;
	}

private:
	pda_state q;
	vector<store_automaton> W;
};

inline ostream& operator<<(ostream& os, const symbolic_config& c) {
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
/**
 * Defining the top of configuration, which is a global state
 */
using top_config = global_state;

}
/* namespace cuba */

#endif /* CUBA_CPDA_HH_ */
