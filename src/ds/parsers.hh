/*
 * parsers.hh
 *
 *  Created on: Jan 1, 2018
 *      Author: lpzun
 */

#ifndef DS_PARSERS_HH_
#define DS_PARSERS_HH_

#include "cpda.hh"
#include "fsa.hh"

namespace ruba {
/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the utilities for PDS file parser.
///
/////////////////////////////////////////////////////////////////////////
class parser {
public:
	static concurrent_pushdown_automata parse_input_cpds(
			const string& filename);
	static concurrent_finite_machine parse_input_cfsm(const string& filename,
			const bool no_pop = false);
	static explicit_config parse_input_cfg(const string& s);

private:
	static vector<vector<string>> read_input_cpds(const string& filename,
			set<pda_state>& states);
	static pushdown_automaton parse_input_pda(const set<pda_state>& states,
			const vector<string>& sPDA);
	static finite_machine parse_input_fsm(const set<pda_state>& states,
			const vector<string>& sPDA);
	static finite_machine parse_input_fsm_no_pop(const vector<string>& sPDA);

	static pda_alpha parse_input_alpha(const string& alpha);

	static void remove_comments(istream& in, ostream& out,
			const string& comment);
	static void remove_comments(istream& in, const string& filename,
			const string& comment);
	static bool getline(istream& in, string& line, const char eol = '\n');

	static deque<string> split(const string &s, const char delim);

	static thread_config create_thread_config_from_str(const string& s_ts,
			const char delim = prop::SHARED_LOCAL_DELIMITER);
	static explicit_config create_global_config_from_str(const string& s_ts,
			const char delim = prop::SHARED_LOCAL_DELIMITER);

	static thread_state create_thread_state_from_str(const string& s_ts,
			const char delim = prop::SHARED_LOCAL_DELIMITER);
	static global_state create_global_state_from_str(const string& s_ts,
			const char delim = prop::SHARED_LOCAL_DELIMITER);
};

class generator {
public:
	generator(const string& initl, const string& filename);
	~generator();

	const vector<set<top_config> >& get_generators() const {
		return generators;
	}

private:
	vector<set<top_config>> generators;

	void context_insensitive(const string& initl, const string& filename);

	void context_insensitive(const top_config& initl, const string& filename);
	vector<set<top_config>> context_insensitive(const top_config& initl_c,
			const vector<finite_machine>& CFSM);
	vector<set<top_config>> standard_FWS(const top_config& initl_c,
			const vector<finite_machine>& CFSM);
	deque<top_config> step(const top_config& c,
			const vector<finite_machine>& CFSM);
	top_config top_mapping(const explicit_config& tau);
	void print_approximation(const vector<set<top_config>>& approx_R);
};
}
/* namespace ruba */

#endif /* DS_PARSERS_HH_ */
