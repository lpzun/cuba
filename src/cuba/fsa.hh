/**
 * @brief fsa.hh
 *
 * @date  : Sep 15, 2016
 * @author: lpzun
 */

#ifndef CUBA_FSA_HH_
#define CUBA_FSA_HH_

#include "pda.hh"

namespace cuba {

using fsa_state = pda_state;
using fsa_alpha = pda_alpha;

/**
 * Definition of finite automaton transition
 */
class fsa_transition {
public:
	fsa_transition();
	fsa_transition(const fsa_state& src, const fsa_state& dst,
			const fsa_alpha& label);
	~fsa_transition();

	fsa_state get_dst() const {
		return dst;
	}

	fsa_alpha get_label() const {
		return label;
	}

	fsa_state get_src() const {
		return src;
	}

private:
	fsa_state src;
	fsa_state dst;
	fsa_alpha label;
};

/**
 * overloading operator <<
 * @param os
 * @param r
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const fsa_transition& r) {
	os << "(" << r.get_src();
	os << "," << r.get_label();
	os << "," << r.get_dst() << ")";
	return os;
}

/**
 * overloading operator <
 * @param r1
 * @param r2
 * @return bool
 */
inline bool operator<(const fsa_transition& r1, const fsa_transition& r2) {
	if (r1.get_src() == r2.get_src()) {
		if (r1.get_dst() == r2.get_dst())
			return r1.get_label() < r2.get_label();
		return r1.get_dst() < r2.get_dst();
	}
	return r1.get_src() < r2.get_src();
}

/**
 * overloading operator >
 * @param r1
 * @param r2
 * @return bool
 */
inline bool operator>(const fsa_transition& r1, const fsa_transition& r2) {
	return r2 < r1;
}

/**
 * overloading operator ==
 * @param r1
 * @param r2
 * @return bool
 */
inline bool operator==(const fsa_transition& r1, const fsa_transition& r2) {
	return (r1.get_src() == r2.get_src()) && (r1.get_dst() == r2.get_dst())
			&& (r1.get_label() == r2.get_label());
}

/**
 * overloading operator !=
 * @param r1
 * @param r2
 * @return bool
 */
inline bool operator!=(const fsa_transition& r1, const fsa_transition& r2) {
	return !(r1 == r2);
}

/// the data structure of FSA transitions
using fsa_delta = unordered_map<fsa_state, set<fsa_transition>>;

/**
 * Definition of finite automaton
 */
class finite_automaton {
public:
	finite_automaton(const fsa_state& state, const fsa_alpha& alpha,
			const fsa_delta& transitions, const fsa_state& accept);

	finite_automaton(const fsa_state& state, const fsa_alpha& alpha,
			const fsa_state& accept);

	~finite_automaton();

	const fsa_delta& get_transitions() const {
		return transitions;
	}

	fsa_state get_accept_state() const {
		return accept_state;
	}

	fsa_alpha get_alphabet() const {
		return alphabet;
	}

	fsa_state get_states() const {
		return states;
	}

	void add_transitions(const fsa_transition& r);

private:
	fsa_state states;        /// 0...states-1
	fsa_alpha alphabet;      ///
	fsa_delta transitions;   ///
	fsa_state accept_state;
};

inline ostream& operator<<(ostream& os, finite_automaton& fsa) {
	for (const auto& p : fsa.get_transitions()) {
		for(const auto& r: p.second) {
			cout << r << "\n";
		}
	}
	return os;
}

} /* namespace cuba */

#endif /* CUBA_FSA_HH_ */
