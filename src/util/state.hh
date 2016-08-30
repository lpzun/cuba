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

#include "utilities.hh"

namespace cuba {

/// the size of threads
using size_t = ushort;

/// control state definition
using control_state = uint;
/// define stack symbol
using stack_symbol = uint;

using id_thread_state = uint;

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

    T top() const {
        if (worklist.empty())
            throw cuba_runtime_error("Stack is empty!");
        return worklist.back();
    }

    void push(const T& _value) {
        worklist.emplace_back(_value);
    }

    T pop() {
        if (worklist.empty())
            throw cuba_runtime_error("Stack is empty!");
        auto res = worklist.back();
        worklist.pop_back();
        return res;
    }

    void overwrite(const T& _value) {
        if (worklist.empty())
            throw cuba_runtime_error("Stack is empty!");
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

class thread_state {
public:
    thread_state();
    thread_state(const control_state& s, const stack_symbol& l);
    ~thread_state();

    static control_state S;
    static stack_symbol L;

    /**
     * @return the stack symbol of current thread state
     */
    stack_symbol get_symbol() const {
        return l;
    }

    /**
     * @return the control state of current thread state
     */
    control_state get_state() const {
        return s;
    }

private:
    control_state s;
    stack_symbol l;
};

/**
 * overloading operator <<
 * @param os
 * @param s
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const thread_state& t) {
    os << "(" << t.get_state() << "," << t.get_symbol() << ")";
    return os;
}

/**
 * overloading operator <
 * @param t1
 * @param t2
 * @return bool
 */
inline bool operator<(const thread_state& t1, const thread_state& t2) {
    return (t1.get_state() < t2.get_state())
            && (t1.get_symbol() < t2.get_symbol());
}

/**
 * overloading operator >
 * @param t1
 * @param t2
 * @return bool
 */
inline bool operator>(const thread_state& t1, const thread_state& t2) {
    return t2 < t1;
}

/**
 * overloading operator ==
 * @param t1
 * @param t2
 * @return bool
 */
inline bool operator==(const thread_state& t1, const thread_state& t2) {
    return (t1.get_state() == t2.get_state())
            && (t1.get_symbol() == t2.get_symbol());
}

/**
 * overloading operator !=
 * @param t1
 * @param t2
 * @return bool
 */
inline bool operator!=(const thread_state& t1, const thread_state& t2) {
    return (t1.get_state() == t2.get_state())
            && (t1.get_symbol() == t2.get_symbol());
}

/// the stack for a single PDS
using sstack = alphabet<stack_symbol>;
/**
 * a configuration (s, w) of a PDS is an element of QxL*.
 */
class thread_config {
public:
    thread_config();
    thread_config(const control_state& s, const stack_symbol& l);
    thread_config(const thread_state& t);
    thread_config(const control_state& s, const sstack& w);
    thread_config(const thread_config& c);
    ~thread_config();

    control_state get_state() const {
        return s;
    }

    const sstack& get_stack() const {
        return w;
    }

    thread_state top() const {
        return thread_state(s, w.top());
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

class global_state {
public:
    global_state(const control_state& s, const size_t& n);
    global_state(const control_state& s, const vector<control_state>& L);
    ~global_state();

    const vector<stack_symbol>& get_local() const {
        return L;
    }

    control_state get_state() const {
        return s;
    }

private:
    control_state s;
    vector<stack_symbol> L;
};

/**
 * overloading operator <<: print a global state
 * @param os
 * @param c
 * @return bool
 */
inline ostream& operator<<(ostream& os, const global_state& g) {
    os << "(" << g.get_state() << "|";
    for (auto i = 0; i < g.get_local().size() - 1; ++i)
        cout << g.get_local()[i] << ",";
    os << g.get_local()[g.get_local().size() - 1] << ")";
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
    global_config(const control_state& s, const size_t& n);
    global_config(const control_state& s, const cstack& W);
    ~global_config();

    control_state get_state() const {
        return s;
    }

    const cstack& get_stacks() const {
        return W;
    }

    global_state top();

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
    for (auto i = 0; i < c.get_stacks().size() - 1; ++i)
        os << c.get_stacks()[i] << ",";
    os << c.get_stacks()[c.get_stacks().size() - 1] << ")";
    return os;
}

using id_transition = uint;

/**
 * define the type of stack operations:
 * PUSH  : push an element to the stack
 * POP   : pop an element from the stack
 * UPDATE: overwrite the top element in the stack
 */
enum class type_stack_operation {
    PUSH, POP, OVERWRITE
};

/**
 * overloading operator <<
 * @param os
 * @param t
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const type_stack_operation& t) {
    switch (t) {
    case type_stack_operation::PUSH:
        os << '+';
        break;
    case type_stack_operation::POP:
        os << '-';
        break;
    default:
        os << '!';
        break;
    }
    return os;
}

/**
 * define type of thread state transitions
 * FORK: spawn a new thread
 * NORM: the normal thread state transitions
 * BRCT: the broadcast transitions
 */
enum class type_synchronization {
    FORK, NORM, BRCT
};

/**
 * overloading operator <<
 * @param os
 * @param t
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const type_synchronization& t) {
    switch (t) {
    case type_synchronization::FORK:
        os << '+';
        break;
    case type_synchronization::BRCT:
        os << '~';
        break;
    default:
        os << '-';
        break;
    }
    return os;
}

/**
 * transition class
 */
template<typename T> class transition {
public:
    transition(const T& src, const T& dst, const type_stack_operation& oper,
            const type_synchronization& sync) :
            src(src), dst(dst), oper(oper), sync(sync) {
    }
    ~transition() {
    }

    T get_dst() const {
        return dst;
    }

    type_stack_operation get_oper_type() const {
        return oper;
    }

    T get_src() const {
        return src;
    }

    type_synchronization get_sync_type() const {
        return sync;
    }

private:
    T src;
    T dst;
    type_stack_operation oper;
    type_synchronization sync;
};

/**
 * overloading the operator <<
 * @param os
 * @param r
 * @return ostream
 */
template<typename T>
inline ostream& operator<<(ostream& os, const transition<T>& r) {
    os << r.get_src() << " ";
    os << r.get_oper_type() << r.get_sync_type() << ">";
    os << " " << r.get_dst();
}

}
/* namespace cuba */

#endif /* UTIL_STATE_HH_ */
