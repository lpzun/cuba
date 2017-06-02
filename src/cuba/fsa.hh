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
/// a set of fsa_state
using fsa_state_set = set<fsa_state>;

using fsa_alpha = pda_alpha;
/// a set of fsa_alpha
using fsa_alphabet = set<fsa_alpha>;

/**
 * Definition of finite automaton transition
 */
class fsa_transition {
public:
	fsa_transition();
	fsa_transition(const fsa_state& src, const fsa_state& dst,
			const fsa_alpha& label);
	~fsa_transition();

	fsa_state get_src() const {
		return src;
	}

	fsa_state get_dst() const {
		return dst;
	}

	fsa_alpha get_label() const {
		return label;
	}

	void set_dst(fsa_state dst) {
		this->dst = dst;
	}

	void set_label(const fsa_alpha label) {
		this->label = label;
	}

	void set_src(const fsa_state src) {
		this->src = src;
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
	os << "(" << r.get_src() << ",";
	if (r.get_label() == -1)
		os << 'e';
	else
		os << r.get_label();
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
	finite_automaton(const fsa_state_set& states, const fsa_alphabet& alphabet,
			const fsa_delta& transitions, const fsa_state_set& initials,
			const fsa_state& accept);

	finite_automaton(const fsa_state_set& states, const fsa_alphabet& alphabet,
			const fsa_state_set& initials, const fsa_state& accept);

	~finite_automaton();

	const fsa_state_set& get_states() const {
		return states;
	}

	const fsa_alphabet& get_alphas() const {
		return alphas;
	}

	const fsa_delta& get_transitions() const {
		return transitions;
	}

	fsa_state_set get_initials() const {
		return initials;
	}

	void set_initials(const fsa_state_set& initials) {
		this->initials = initials;
	}

	fsa_state get_accept() const {
		return accept;
	}

private:
	fsa_state_set states;    /// it represents state 0...|states|-1
	fsa_alphabet alphas;   ///
	fsa_delta transitions;   ///

	fsa_state_set initials;  /// it represents state 0...|initials|-1
	fsa_state accept;  /// accept state
};

/**
 * overloading operator <<
 * @param os
 * @param fsa
 * @return ostream
 */
inline ostream& operator<<(ostream& os, const finite_automaton& fsa) {
	if (fsa.get_states().size() == 0)
		return os;

	/*
	 /// matrix ...
	 vector<vector<string>> matrix(
	 fsa.get_states().size() + fsa.get_initials().size(),
	 vector<string>(fsa.get_states().size(), ""));
	 for (const auto& p : fsa.get_transitions()) {
	 for (const auto& r : p.second) {
	 const int& i = r.get_src();
	 const int& j = r.get_dst() - fsa.get_initials().size();
	 if (matrix[i][j].length() > 0)
	 matrix[i][j].push_back(',');
	 if (r.get_label() == -1)
	 matrix[i][j] += "e";
	 else
	 matrix[i][j] += std::to_string(r.get_label());
	 }
	 }

	 auto m = matrix.size();
	 auto n = std::to_string(m).length() + 1;
	 os << algs::widthify("", n + 2);
	 for (int i = fsa.get_initials().size(); i < m; ++i) {
	 os << algs::widthify("q" + std::to_string(i), n, alignment::LEFTJUST);
	 os << " | ";
	 }
	 os << "\n";
	 for (int i = 0; i < m; ++i) {
	 os << algs::widthify("q" + std::to_string(i), n, alignment::LEFTJUST);
	 os << "|";
	 for (int j = 0; j < matrix[0].size(); ++j)
	 os << algs::widthify(matrix[i][j], n + 2, alignment::CENTERED)
	 << " ";
	 os << "\n";
	 }
	 */
	os << "states: ";
	for (auto q : fsa.get_initials())
		os << "q" << q << " ";
	for (auto s : fsa.get_states())
		os << "s" << s << " ";
	os << "\n";
	os << "accept: " << fsa.get_accept() << "\n";
	for (auto p : fsa.get_transitions()) {
		for (auto r : p.second)
			os << r << "\n";
	}
	return os;
}

} /* namespace cuba */

#endif /* CUBA_FSA_HH_ */
