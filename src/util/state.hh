/******************************************************************************
 * @brief state.hh
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

#ifndef UTIL_STATE_HH_
#define UTIL_STATE_HH_

#include "heads.hh"

namespace cuba {

/// control state definition
using control_state = unsigned int;
/// define stack symbol
using stack_symbol = unsigned int;
/// define input alphabet
using alphabet = deque<stack_symbol>;

/**
 * a configuration (s, w) of a PDS is an element of QxL*.
 */
class thread_config {
public:
    thread_config();
    thread_config(const control_state& s, const alphabet& w);
    thread_config(const thread_config& c);
    ~thread_config();

    control_state get_state() const {
        return s;
    }

    const alphabet& get_stack() const {
        return w;
    }

private:
    control_state s; /// control state
    alphabet w; /// stack content
};

/**
 * overloading operator <<: print thread configuration
 *
 * @param out
 * @param c
 * @return ostream
 */
inline ostream& operator<<(ostream& out, const thread_config& c) {
    out << "(" << c.get_state() << "," << c.get_stack() << ")";
    return out;
}

inline/**
 * a configuration (s|w1,...,wn) of a CPDS is an element of Qx(L*)^n
 */
class global_config {
public:

};

} /* namespace bssp */

#endif /* UTIL_STATE_HH_ */
