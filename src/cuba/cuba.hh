/**
 * @brief cuba.hh
 *
 * @date  : Aug 28, 2016
 * @author: lpzun
 */

#ifndef CUBA_CUBA_HH_
#define CUBA_CUBA_HH_

#include "../utils/prop.hh"

#include "cpda.hh"
#include "fsa.hh"

namespace cuba {

using vertex = id_thread_state;
using edge = id_transition;
using adj_list = map<vertex, deque<edge>>;

using antichain = deque<global_config>;

using pda_rule = pda_transition<vertex>;

/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the declarations for context-unbounded
/// analysis.
/////////////////////////////////////////////////////////////////////////
class CUBA {
public:
	CUBA(const string& filename, const string& initl, const string& final);
	~CUBA();

	void context_bounded_analysis(const size_t& n, const size_k& k);

private:
	/// Part 1: parse a pushdown system (PDS)
	vector<vector<vertex>> mapping_Q; /// mapping a control state to its ID
	vector<thread_state> active_Q;       /// active control states
	vector<pda_rule> active_R; /// active transitions
	adj_list PDS;

	thread_config initl_c;
	thread_config final_c;

	void parse_input_pda(const string& filename);
	thread_config parse_input_cfg(const string& s);

	/// build reachability automaton
	finite_automaton create_reachability_automaton();
	finite_automaton create_init_fsa(const thread_config& c);
	finite_automaton post_kleene(const finite_automaton& A);
	vertex retrieve(const pda_state& s, const pda_alpha& l);
	bool is_recongnizable(const finite_automaton& fsa, const thread_config& c);
	bool is_equivalent(const finite_automaton& fsa1,
			const finite_automaton& fsa2);

	deque<aggregate_config> context_bounded_analysis(const size_t& n,
			const size_k& k, const pda_state& g_in,
			const finite_automaton& A_in);

	deque<fsa_state> project_G(const finite_automaton& A);
	deque<fsa_state> BFS_visit(const fsa_state& root,
			const unordered_map<fsa_state, deque<fsa_state>>& adj,
			const fsa_state_set& initials);
	aggregate_config compose(const pda_state& _g,
			const vector<finite_automaton>& automatons, const int& idx);
	finite_automaton rename(const finite_automaton& fsa, const pda_state& _g);
	finite_automaton anonymize(const finite_automaton& fsa, const pda_state& _g,
			const bool& is_rename = false);
	finite_automaton anonymize_by_delete(const finite_automaton& fsa,
			const pda_state& _g);
	finite_automaton anonymize_by_rename(const finite_automaton& fsa,
			const pda_state& _g);
};

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following are the utilities for PDS file parser.
///
/////////////////////////////////////////////////////////////////////////
class pds_parser {
public:
	static void remove_comments(istream& in, const string& filename,
			const string& comment);
	static void remove_comments(const string& in, string& out,
			const string& comment);

	static thread_config create_thread_config_from_str(const string& s_ts,
			const char& delim = '|');
	static global_config create_global_config_from_str(const string& s_ts,
			const char& delim = '|');

	static thread_state create_thread_state_from_str(const string& s_ts,
			const char& delim = '|');
	static global_state create_global_state_from_str(const string& s_ts,
			const char& delim = '|');

	static type_stack_operation parse_type_stack_operation(const char& c);
	static type_synchronization parse_type_synchronization(const char& c);

private:
	static void remove_comments(istream& in, ostream& out,
			const string& comment);
	static bool getline(istream& in, string& line, const char& eol = '\n');

	static deque<string> split(const string &s, const char& delim);

};

/////////////////////////////////////////////////////////////////////////
/// PART 3. The following are the utilities for explore.
///
/////////////////////////////////////////////////////////////////////////
/**
 * The class of explore:
 *     define a tool like explore in CAV'10 dynamic cutoff tool
 */
class simulator {
public:
	simulator(const ctx_bound& k, const thread_state& initl,
			const thread_state& final, ///
			const vector<vector<vertex>>& mapping_Q,
			const vector<thread_state>& Q,
			const vector<pda_transition<vertex>>& R);

	~simulator();

	void context_bounded_analysis(const size_t& n, const size_k& k);
private:
	///  Part 1: parse a pushdown system (PDS)
	ctx_bound k_bound;
	thread_state initl_TS;
	thread_state final_TS;

	vector<vector<vertex>> mapping_Q; /// mapping a control state to its ID
	vector<thread_state> active_Q;       /// active control states
	vector<pda_transition<vertex>> active_R; /// active transitions
	adj_list PDS;

	vector<vector<bool>> reachable_T;

	uint bounded_reachability(const size_t& n, const size_k& k);

	antichain step(const global_config& tau);
	bool is_reachable(const global_config& tau, antichain& R);
	void marking(const pda_state& s, const pda_alpha& l);
	vertex retrieve(const pda_state& s, const pda_alpha& l);
};

} /* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
