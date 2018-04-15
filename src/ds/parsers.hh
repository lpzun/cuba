/**
 * PDS parser
 *
 *  @date: Jan 1, 2018
 *  @author: TODO
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
	static concurrent_finite_machine parse_input_cfsm(const string& filename);
	static explicit_state parse_input_cfg(const string& s);

	static string create_default_states_in_str(const size_n n);

private:
	static vector<vector<string>> read_input_cpds(const string& filename,
			set<pda_state>& states);
	static pushdown_automaton parse_input_pda(const set<pda_state>& states,
			const vector<string>& sPDA);
	static finite_machine parse_input_fsm(const vector<string>& sPDA);

	static pda_alpha parse_input_alpha(const string& alpha);

	static void remove_comments(istream& in, ostream& out,
			const string& comment);
	static void remove_comments(istream& in, const string& filename,
			const string& comment);
	static bool getline(istream& in, string& line, const char eol = '\n');

	static deque<string> split(const string &s, const char delim);

	static thread_state create_thread_config_from_str(const string& s_ts,
			const char delim = prop::SHARED_LOCAL_DELIMITER);
	static explicit_state create_global_config_from_str(const string& s_ts,
			const char delim = prop::SHARED_LOCAL_DELIMITER);

	static thread_visible_state create_thread_state_from_str(const string& s_ts,
			const char delim = prop::SHARED_LOCAL_DELIMITER);
	static visible_state create_global_state_from_str(const string& s_ts,
			const char delim = prop::SHARED_LOCAL_DELIMITER);
};
}
/* namespace ruba */

#endif /* DS_PARSERS_HH_ */
