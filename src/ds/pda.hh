/**
 * @brief pds.hh
 *
 * @date  : Sep 15, 2016
 * @author: lpzun
 */

#ifndef DS_PDA_HH_
#define DS_PDA_HH_

#include "utilities.hh"
#include "prop.hh"

namespace ruba {

/// define the control state of PDSs
using pda_state = unsigned int;
/// define the stack symbol of PDSs
using pda_alpha = unsigned int;

/////////////////////////////////////////////////////////////////////////
/// PART 1. alphabet and PDA stack's definitions are from here.
///
/// alphabet:
/// PDA stack: PDA stack
///
/////////////////////////////////////////////////////////////////////////
/// Definition of alphabet
class alphabet {
public:
	alphabet() {
	}
	~alphabet() {
	}

	const static pda_alpha EPSILON;
	const static pda_alpha NULLPTR;
	const static char OPT_EPSILON;
};

/**
 * define the type of stack operations:
 * PUSH  : push an element to the stack
 * POP   : pop an element from the stack
 * OVERWRITE: overwrite the top element of the stack
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
		os << '-';
		break;
	case type_stack_operation::POP:
		os << '-';
		break;
	default:
		os << '-';
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
 * PDA transition class
 */
template<typename T1, typename T2> class transition {
public:
	transition(const T1& src, const T2& dst, const type_stack_operation& op) :
			src(src), dst(dst), op(op) {
	}
	~transition() {
	}

	T1 get_src() const {
		return src;
	}

	T2 get_dst() const {
		return dst;
	}

	type_stack_operation get_oper_type() const {
		return op;
	}

private:
	T1 src;
	T2 dst;
	type_stack_operation op;
};

/**
 * overloading the operator <<
 * @param os
 * @param r
 * @return ostream
 */
template<typename T1, typename T2> inline ostream& operator<<(ostream& os,
		const transition<T1, T2>& r) {
	os << r.get_src() << " ";
	os << "->";
	os << " " << r.get_dst();
	return os;
}

/////////////////////////////////////////////////////////////////////////
/// PART 2. Thread state and thread configuration definitions are from
/// here.
///
/// thread state:
/// thread configuration: a configuration of PDA
///
/////////////////////////////////////////////////////////////////////////

/**
 * the thread state class
 */
class thread_visible_state {
public:
	thread_visible_state();
	thread_visible_state(const pda_state& s, const pda_alpha& l);
	~thread_visible_state();

	static pda_state S;

	/**
	 * @return the stack symbol of current thread state
	 */
	pda_alpha get_alpha() const {
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
inline ostream& operator<<(ostream& os, const thread_visible_state& t) {
	os << "(" << t.get_state() << ",";
	if (t.get_alpha() == alphabet::EPSILON)
		os << alphabet::OPT_EPSILON;
	else
		os << t.get_alpha();
	os << ")";
	return os;
}

/**
 * overloading operator <
 * @param t1
 * @param t2
 * @return bool
 */
inline bool operator<(const thread_visible_state& t1, const thread_visible_state& t2) {
	if (t1.get_state() == t2.get_state())
		return t1.get_alpha() < t2.get_alpha();
	return t1.get_state() < t2.get_state();
}

/**
 * overloading operator >
 * @param t1
 * @param t2
 * @return bool
 */
inline bool operator>(const thread_visible_state& t1, const thread_visible_state& t2) {
	return t2 < t1;
}

/**
 * overloading operator ==
 * @param t1
 * @param t2
 * @return bool
 */
inline bool operator==(const thread_visible_state& t1, const thread_visible_state& t2) {
	return (t1.get_state() == t2.get_state())
			&& (t1.get_alpha() == t2.get_alpha());
}

/**
 * overloading operator !=
 * @param t1
 * @param t2
 * @return bool
 */
inline bool operator!=(const thread_visible_state& t1, const thread_visible_state& t2) {
	return !(t1 == t2);
}

/**
 * Definition of the stack of PDA
 */
template<typename T> class sstack {
public:
	inline sstack() :
			worklist() {
	}

	inline sstack(const deque<T>& worklist) :
			worklist(worklist) {
	}

	inline ~sstack() {
	}

	T top() {
		return worklist.front();
	}

	T top() const {
		if (worklist.empty())
			throw cuba_runtime_error("Stack is empty!");
		return worklist.front();
	}

	void push(const T& _value) {
		worklist.emplace_front(_value);
	}

	size_t size() const {
		return worklist.size();
	}

	bool pop() {
		if (worklist.empty())
			return false;
		worklist.pop_front();
		return true;
	}

	bool overwrite(const T& _value) {
		if (worklist.empty())
			return false;
		worklist[0] = _value;
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
		const sstack<T>& a) {
	if (a.size() == 0) {
		os << alphabet::OPT_EPSILON;
	} else {
		for (const T& s : a.get_worklist()) {
			if (s == alphabet::EPSILON)
				os << alphabet::OPT_EPSILON;
			else
				os << s;
		}
	}
	return os;
}

/**
 * overloading operator <
 * @param a1
 * @param a2
 * @return bool
 */
template<typename T> inline bool operator<(const sstack<T>& a1,
		const sstack<T>& a2) {
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
template<typename T> inline bool operator>(const sstack<T>& a1,
		const sstack<T>& a2) {
	return a2 < a1;
}

/**
 * overloading operator ==
 * @param a1
 * @param a2
 * @return bool
 */
template<typename T> inline bool operator==(const sstack<T>& a1,
		const sstack<T>& a2) {
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
template<typename T> inline bool operator!=(const sstack<T>& a1,
		const sstack<T>& a2) {
	return !(a1 == a2);
}

/**
 * Definition of the stack for a PDA
 */
using pda_stack = sstack<pda_alpha>;

/**
 * a configuration (s, w) of a PDS is an element of Q x L*.
 */
class thread_state {
public:
	thread_state();
	thread_state(const pda_state& s, const pda_alpha& l);
	thread_state(const thread_visible_state& t);
	thread_state(const pda_state& s, const pda_stack& w);
	thread_state(const thread_state& c);
	~thread_state();

	pda_state get_state() const {
		return s;
	}

	const pda_stack& get_stack() const {
		return w;
	}

	thread_visible_state top() const {
		return thread_visible_state(s, w.top());
	}

private:
	pda_state s; /// control state
	pda_stack w; /// stack content
};

/**
 * overloading operator <<: print thread configuration
 *
 * @param out
 * @param c
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const thread_state& c) {
	os << "(" << c.get_state() << "," << c.get_stack() << ")";
	return os;
}

/**
 * overloading operator <
 * @param c1
 * @param c2
 * @return bool
 */
inline bool operator<(const thread_state& c1, const thread_state& c2) {
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
inline bool operator>(const thread_state& c1, const thread_state& c2) {
	return c2 < c1;
}

/**
 * overloading operator ==
 * @param c1
 * @param c2
 * @return bool
 */
inline bool operator==(const thread_state& c1, const thread_state& c2) {
	return (c1.get_state() == c2.get_state())
			&& (c1.get_stack() == c2.get_stack());
}

/**
 * overloading operator !=
 * @param c1
 * @param c2
 * @return bool
 */
inline bool operator!=(const thread_state& c1, const thread_state& c2) {
	return !(c1 == c2);
}

/////////////////////////////////////////////////////////////////////////
/// PART 3. The definition of a sequential pushdown automaton
///
/////////////////////////////////////////////////////////////////////////
/// define the transition ID
using id_action = uint;
using vertex = thread_visible_state;
using edge = id_action;
using adj_list = map<vertex, deque<edge>>;
using pda_action = transition<vertex, thread_state>;

/**
 * Definition of Pushdown automaton
 */
class pushdown_automaton {
public:
	pushdown_automaton();
	pushdown_automaton(const set<pda_state>& states,
			const set<pda_alpha>& alphas, ///
			const vector<pda_action>& actions, ///
			const adj_list& PDA);
	~pushdown_automaton();

	const set<pda_state>& get_states() const {
		return states;
	}

	const set<pda_alpha>& get_alphas() const {
		return alphas;
	}

	const vector<pda_action>& get_actions() const {
		return actions;
	}

	const adj_list& get_program() const {
		return program;
	}

private:
	set<pda_state> states; /// the set of control states
	set<pda_alpha> alphas; /// the set of stack symbols
	vector<pda_action> actions; /// the set of actions
	adj_list program; /// store PDA in adjacency list
};

/**
 * overloading operator <<: print a PDA
 * @param os
 * @param PDA
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const pushdown_automaton& PDA) {
	for (auto s : PDA.get_states()) {
		os << s << " ";
	}
	os << "\n";
	for (auto r : PDA.get_actions()) {
		os << r << "\n";
	}
	os << "\n";
	return os;
}

} /* namespace ruba */

#endif /* DS_PDA_HH_ */
