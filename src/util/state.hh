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

/// the size of threads
using size_t = unsigned short;

/// control state definition
using control_state = unsigned int;
/// define stack symbol
using stack_symbol = unsigned int;

template<typename T> class alphabet {
public:
    inline alphabet() :
            worklist() {
    }

    inline alphabet(const deque<T>& worklist) :
            worklist(worklist) {
    }

    inline ~alphabet() {
    }

    T top() {
        return worklist.back();
    }

    void push(const T& _value) {
        worklist.emplace_back(_value);
    }

    T pop() {
        if (worklist.empty())
            throw;
        auto res = worklist.back();
        worklist.pop_back();
        return res;
    }

    void overwrite(const T& _value) {
        if (worklist.empty())
            throw;
        worklist.pop_back();
        worklist.push_back(_value);
    }

    bool empty() const {
        return worklist.empty();
    }

    const deque<T>& get_worklist() const {
        return worklist;
    }

private:
    deque<T> worklist;
};

/**
 * overloading operator <<: ostream
 * @param os
 * @param a
 * @return ostream
 */
template<typename T> inline ostream& operator<<(ostream& os,
        const alphabet<T>& a) {
    for (const T& s : a.get_worklist())
        os << s;
    return os;
}

/**
 * overloading operator <
 * @param a1
 * @param a2
 * @return bool
 */
template<typename T> inline bool operator<(const alphabet<T>& a1,
        const alphabet<T>& a2) {
    if (a1.get_worklist().size() == a2.get_worklist().size()) {
        auto ia1 = a1.get_worklist().cbegin();
        auto ia2 = a2.get_worklist().cbegin();
        while (ia1 != a1.get_worklist().cend()) {
            if (*ia1 < *ia2) {
                return true;
            } else if (*ia1 > *ia2) {
                return false;
            } else {
                ++ia1, ++ia2;
            }
        }
        return false;
    }
    return a1.get_worklist().size() < a2.get_worklist().size();
}

/**
 * overloading operator >
 * @param a1
 * @param a2
 * @return bool
 */
template<typename T> inline bool operator>(const alphabet<T>& a1,
        const alphabet<T>& a2) {
    return a2 < a1;
}

/**
 * overloading operator ==
 * @param a1
 * @param a2
 * @return bool
 */
template<typename T> inline bool operator==(const alphabet<T>& a1,
        const alphabet<T>& a2) {
    if (a1.get_worklist().size() != a2.get_worklist().size())
        return false;
    auto iter1 = a1.get_worklist().cbegin();
    auto iter2 = a2.get_worklist().cbegin();
    while (iter1 != a1.get_worklist().cend()) {
        if (*iter1 != *iter2)
            return false;
        ++iter1, ++iter2;
    }
    return true;
}

/**
 * overloading operator !=
 * @param a1
 * @param a2
 * @return bool
 */
template<typename T> inline bool operator!=(const alphabet<T>& a1,
        const alphabet<T>& a2) {
    return !(a1 == a2);
}

/// the stack for a single PDS
using sstack = alphabet<stack_symbol>;
/**
 * a configuration (s, w) of a PDS is an element of QxL*.
 */
class thread_config {
public:
    thread_config();
    thread_config(const control_state& s, const sstack& w);
    thread_config(const thread_config& c);
    ~thread_config();

    control_state get_state() const {
        return s;
    }

    const sstack& get_stack() const {
        return w;
    }

private:
    control_state s; /// control state
    sstack w; /// stack content
};

/**
 * overloading operator <<: print thread configuration
 *
 * @param out
 * @param c
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const thread_config& c) {
    os << "(" << c.get_state() << "," << c.get_stack() << ")";
    return os;
}

/**
 * overloading operator <
 * @param c1
 * @param c2
 * @return bool
 */
inline bool operator<(const thread_config& c1, const thread_config& c2) {
    if (c1.get_state() == c2.get_state())
        return c1.get_stack() < c2.get_stack();
    return c1.get_state() < c2.get_state();
}

/**
 * overloading operator >
 * @param c1
 * @param c2
 * @return bool
 */
inline bool operator>(const thread_config& c1, const thread_config& c2) {
    return c2 < c1;
}

/**
 * overloading operator ==
 * @param c1
 * @param c2
 * @return bool
 */
inline bool operator==(const thread_config& c1, const thread_config& c2) {
    return (c1.get_state() == c2.get_state())
            && (c1.get_stack() == c2.get_stack());
}

/**
 * overloading operator !=
 * @param c1
 * @param c2
 * @return bool
 */
inline bool operator!=(const thread_config& c1, const thread_config& c2) {
    return !(c1 == c2);
}

/// the set of stacks in CPDS
using cstack = vector<sstack>;

/**
 * a configuration (s|w1,...,wn) of a CPDS is an element of Qx(L*)^n
 */
class global_config {
public:
    global_config(const control_state& s, const size_t& n);
    global_config(const control_state& s, const cstack& W);
    ~global_config();

    control_state get_state() const {
        return s;
    }

    const cstack& get_stacks() const {
        return W;
    }

private:
    control_state s;
    cstack W;
};

/**
 * overloading operator <<: print a configuration of the CPDS
 * @param os
 * @param c
 * @return bool
 */
inline ostream& operator<<(ostream& os, const global_config& c) {
    os << "(" << c.get_state() << "|";
    for (const auto& w : c.get_stacks())
        os << w << ",";
    os << ")";
    return os;
}

} /* namespace bssp */

#endif /* UTIL_STATE_HH_ */
