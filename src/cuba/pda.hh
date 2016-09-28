/**
 * @brief pds.hh
 *
 * @date  : Sep 15, 2016
 * @author: lpzun
 */

#ifndef CUBA_PDA_HH_
#define CUBA_PDA_HH_

#include "utilities.hh"

namespace cuba {

class alphabet {
public:
	alphabet() {
	}
	~alphabet() {
	}

	/**
	 * REMARK: Epsilon is -1, all other stack symbol, aka pda_alpha
	 * is a natural number
	 */
	static int EPSILON;
};

/// define the control state of PDSs
using pda_state = int;
/// define the stack symbol of PDSs
using pda_alpha = int;

/**
 * define the stack of PDSs
 */
template<typename T> class pda_stack {
public:
	inline pda_stack() :
			worklist() {
	}

	inline pda_stack(const deque<T>& worklist) :
			worklist(worklist) {
	}

	inline ~pda_stack() {
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

	bool pop() {
		if (worklist.empty())
			return false;
		worklist.pop_back();
		return true;
	}

	bool overwrite(const T& _value) {
		if (worklist.empty())
			return false;
		worklist[worklist.size() - 1] = _value;
		return true;
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
		const pda_stack<T>& a) {
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
template<typename T> inline bool operator<(const pda_stack<T>& a1,
		const pda_stack<T>& a2) {
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
template<typename T> inline bool operator>(const pda_stack<T>& a1,
		const pda_stack<T>& a2) {
	return a2 < a1;
}

/**
 * overloading operator ==
 * @param a1
 * @param a2
 * @return bool
 */
template<typename T> inline bool operator==(const pda_stack<T>& a1,
		const pda_stack<T>& a2) {
	if (a1.get_worklist().size() != a2.get_worklist().size())
		return false;
	auto ia1 = a1.get_worklist().cbegin();
	auto ia2 = a2.get_worklist().cbegin();
	while (ia1 != a1.get_worklist().cend()) {
		if (*ia1 != *ia2)
			return false;
		++ia1, ++ia2;
	}
	return true;
}

/**
 * overloading operator !=
 * @param a1
 * @param a2
 * @return bool
 */
template<typename T> inline bool operator!=(const pda_stack<T>& a1,
		const pda_stack<T>& a2) {
	return !(a1 == a2);
}

/**
 * the thread state class
 */
class thread_state {
public:
	thread_state();
	thread_state(const pda_state& s, const pda_alpha& l);
	~thread_state();

	static pda_state S;
	static pda_alpha L;

	/**
	 * @return the stack symbol of current thread state
	 */
	pda_alpha get_symbol() const {
		return l;
	}

	/**
	 * @return the control state of current thread state
	 */
	pda_state get_state() const {
		return s;
	}

private:
	pda_state s;
	pda_alpha l;
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
	if (t1.get_state() == t2.get_state())
		return t1.get_symbol() < t2.get_symbol();
	return t1.get_state() < t2.get_state();
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
	return !(t1 == t2);
}

/// the stack for a single PDS
using sstack = pda_stack<pda_alpha>;
/**
 * a configuration (s, w) of a PDS is an element of QxL*.
 */
class thread_config {
public:
	thread_config();
	thread_config(const pda_state& s, const pda_alpha& l);
	thread_config(const thread_state& t);
	thread_config(const pda_state& s, const sstack& w);
	thread_config(const thread_config& c);
	~thread_config();

	pda_state get_state() const {
		return s;
	}

	const sstack& get_stack() const {
		return w;
	}

	thread_state top() const {
		return thread_state(s, w.top());
	}

private:
	pda_state s; /// control state
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
template<typename T> class pda_transition {
public:
	pda_transition(const T& src, const T& dst, const type_stack_operation& oper,
			const type_synchronization& sync) :
			src(src), dst(dst), oper(oper), sync(sync) {
	}
	~pda_transition() {
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
inline ostream& operator<<(ostream& os, const pda_transition<T>& r) {
	os << r.get_src() << " ";
	os << r.get_oper_type() << r.get_sync_type() << ">";
	os << " " << r.get_dst();
	return os;
}

/**
 * Definition of Pushdown automaton
 */
class pushdown_automaton {
public:
	pushdown_automaton();
	~pushdown_automaton();
};

} /* namespace cuba */

#endif /* CUBA_PDA_HH_ */
