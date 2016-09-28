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

namespace cuba {

/// the size of threads
using size_t = ushort;
/// the size of context switches
using size_k = ushort;

using id_thread_state = uint;

using id_thread = size_t;
using ctx_bound = size_k;

/// the set of stacks in CPDS
using stack_vec = vector<sstack>;

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
	os << "(" << g.get_state() << "|";
	if (g.get_local().size() > 0)
		os << g.get_local()[0];
	for (auto i = 1; i < g.get_local().size(); ++i)
		cout << "," << g.get_local()[i];
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
		return algs::compare(g1.get_local(), g2.get_local(), true) == -1;
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
		return algs::compare(g1.get_local(), g2.get_local(), true) == 0;
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
class global_config {
public:
	global_config(const pda_state& s, const size_t& n);
	global_config(const id_thread& id, const ctx_bound& k,
			const pda_state& s, const size_t& n);
	global_config(const id_thread& id, const ctx_bound& k,
			const pda_state& s, const stack_vec& W);
	global_config(const global_config& g);
	~global_config();

	pda_state get_state() const {
		return s;
	}

	const stack_vec& get_stacks() const {
		return W;
	}

	ctx_bound get_context_k() const {
		return k;
	}

	void set_context_k(const ctx_bound& k) {
		this->k = k;
	}

	id_thread get_thread_id() const {
		return id;
	}

	global_state top();
	global_state top() const;

private:
	id_thread id; /// the active thread that reach current configuration
	ctx_bound k; /// the number of context switches used to reach current configuration
	pda_state s;
	stack_vec W;
};

/**
 * overloading operator <<: print a configuration of the CPDS
 * @param os
 * @param c
 * @return bool
 */
inline ostream& operator<<(ostream& os, const global_config& c) {
	os << "(" << c.get_thread_id() << "," << c.get_context_k() << ",";
	os << c.get_state() << "|";
	if (c.get_stacks().size() > 0)
		os << c.get_stacks()[0];
	for (int i = 1; i < c.get_stacks().size(); ++i)
		os << "," << c.get_stacks()[i];
	os << ")";
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
	if (g1.get_thread_id() == g2.get_thread_id()
			&& g1.get_state() == g2.get_state()) {
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

}
/* namespace cuba */

#endif /* CUBA_CPDA_HH_ */
