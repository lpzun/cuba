/**
 * @brief cuba.hh
 *
 * @date  : Aug 28, 2016
 * @author: lpzun
 */

#ifndef CUBA_CUBA_HH_
#define CUBA_CUBA_HH_

#include "prop.hh"

#include "cpda.hh"
#include "fsa.hh"

namespace cuba {

/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the declarations for context-unbounded
/// analysis.
/////////////////////////////////////////////////////////////////////////
class CUBA {
public:
	CUBA(const string& initl, const string& final, const string& filename);
	~CUBA();

	void context_bounded_analysis(const size_k& k, const size_t& n = 0);

private:
	concrete_config initl_c;
	concrete_config final_c;
	concurrent_pushdown_automata CPDA;

	/// Post*(A): build pushdown store automaton
	store_automaton create_store_automaton(const size_t i);
	store_automaton create_init_automaton(const size_t i);
	fsa_state create_accept_state(const fsa_state_set& states);
	fsa_state create_interm_state(const fsa_state_set& states);
	store_automaton post_kleene(const store_automaton& A,
			const pushdown_automaton& P);

	/// QR algorithm: context-bounded analysis
	vector<deque<symbolic_config>> context_bounded_analysis(const size_k k,
			const symbolic_config& cfg_I);
	set<fsa_state> project_Q(const store_automaton& A);
	set<fsa_state> BFS_visit(const fsa_state& root,
			const unordered_map<fsa_state, deque<fsa_state>>& adj,
			const fsa_state_set& initials);
	symbolic_config compose(const pda_state& _q, const store_automaton& Ai,
			const vector<store_automaton>& automata, const size_t& idx);
	store_automaton rename(const store_automaton& A, const pda_state& q_I);
	store_automaton anonymize(const store_automaton& A, const pda_state& q_I,
			const bool& is_rename = false);
	store_automaton anonymize_by_split(const store_automaton& A,
			const pda_state& q_I);
	store_automaton anonymize_by_rename(const store_automaton& A,
			const pda_state& q_I);

	/// determine R_k = R_{k+1}
	bool is_recongnizable(const store_automaton& A, const thread_config& c);
	bool is_equivalent(const store_automaton& A1, const store_automaton& A2);
	store_automaton iunion(const store_automaton& A1,
			const store_automaton& A2);
	store_automaton intersect(const store_automaton& A1,
			const store_automaton& A2);
	store_automaton complement(const store_automaton& A);
	store_automaton cross_product(const vector<store_automaton>& W);

	/// determine bar(R_k) = bar(R_{k+1})s
	int top_mapping(const deque<symbolic_config>& global_R,
			vector<set<config_top>>& topped_R);
	vector<config_top> top_mapping(const symbolic_config& tau);
	set<pda_alpha> top_mapping(const store_automaton& A, const pda_state q);
	vector<vector<pda_alpha>> cross_product(const vector<set<pda_alpha>>& tops);
};

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following are the utilities for explore.
///
/////////////////////////////////////////////////////////////////////////

using antichain = deque<global_config>;

/**
 * The class of explore:
 *     define a tool like explore in CAV'10 dynamic cutoff tool
 */
class simulator {
public:
	simulator(const string& initl, const string& final, const string& filename);

	~simulator();

	void context_bounded_analysis(const size_k& k, const size_t& n = 0);
private:
	///  Part 1: parse a pushdown system (PDS)
	concrete_config initl_c;
	concrete_config final_c;
	concurrent_pushdown_automata CPDA;
	vector<vector<bool>> reachable_T;

	bool k_bounded_reachability(const size_k k_bound,
			const concrete_config& c_I);

	antichain step(const global_config& tau, const size_k k_bound);
	bool is_reachable(const global_config& tau, vector<vector<antichain>>& R);
	void marking(const pda_state& s, const pda_alpha& l);

	config_top top_mapping(const global_config& tau);

	/// cycle detection
	bool is_cyclic(const size_t tid);
	bool is_cyclic(const size_t tid, const thread_state& s, stack<pda_alpha>& W,
			map<vertex, bool>& visit, map<vertex, bool>& trace);
};

/////////////////////////////////////////////////////////////////////////
/// PART 3. The following are the utilities for PDS file parser.
///
/////////////////////////////////////////////////////////////////////////
class parser {
public:
	static concurrent_pushdown_automata parse_input_cpds(
			const string& filename);

	static concrete_config parse_input_cfg(const string& s);

	static type_stack_operation parse_type_stack_operation(const char& c);
	static type_synchronization parse_type_synchronization(const char& c);

private:
	static pushdown_automaton parse_input_pda(const set<pda_state>& states,
			const vector<string>& sPDA);

	static void remove_comments(istream& in, ostream& out,
			const string& comment);
	static bool getline(istream& in, string& line, const char& eol = '\n');

	static deque<string> split(const string &s, const char& delim);

	static void remove_comments(istream& in, const string& filename,
			const string& comment);
	static void remove_comments(const string& in, string& out,
			const string& comment);

	static thread_config create_thread_config_from_str(const string& s_ts,
			const char& delim = prop::SHARED_LOCAL_DELIMITER);
	static concrete_config create_global_config_from_str(const string& s_ts,
			const char& delim = prop::SHARED_LOCAL_DELIMITER);

	static thread_state create_thread_state_from_str(const string& s_ts,
			const char& delim = prop::SHARED_LOCAL_DELIMITER);
	static global_state create_global_state_from_str(const string& s_ts,
			const char& delim = prop::SHARED_LOCAL_DELIMITER);

};

} /* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
