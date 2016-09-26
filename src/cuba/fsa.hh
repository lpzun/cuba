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

using fsa_state = int;
using fsa_alpha = int;

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
 * overloading
 * @param os
 * @param r
 * @return
 */
inline ostream& operator<<(ostream& os, const fsa_transition& r) {
	os << "(" << r.get_src();
	os << "," << r.get_label();
	os << "," << r.get_dst() << ")";
	return os;
}

/**
 * Definition of finite automaton
 */
class finite_automaton {
public:
	finite_automaton();
	~finite_automaton();

	const vector<deque<fsa_transition>>& get_transitions() const {
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

	void add_transitions(const fsa_transition& r) {
		this->transitions[r.get_src()].push_back(r);
	}

private:
	fsa_state states; // 0...states-1
	fsa_alpha alphabet; //
	vector<deque<fsa_transition>> transitions;
	fsa_state accept_state;
	set<fsa_state> intermediate_states;
};

} /* namespace cuba */

#endif /* CUBA_FSA_HH_ */
