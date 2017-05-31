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

using antichain = deque<concrete_config>;

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
	deque<symbolic_config> context_bounded_analysis(const size_k k,
			const symbolic_config& cfg_I);
	deque<fsa_state> project_G(const store_automaton& A);
	deque<fsa_state> BFS_visit(const fsa_state& root,
			const unordered_map<fsa_state, deque<fsa_state>>& adj,
			const fsa_state_set& initials);
	symbolic_config compose(const pda_state& _q,
			const vector<store_automaton>& automatons, const int& idx);
	store_automaton rename(const store_automaton& A, const pda_state& q_I);
	store_automaton anonymize(const store_automaton& A, const pda_state& q_I,
			const bool& is_rename = false);
	store_automaton anonymize_by_split(const store_automaton& A,
			const pda_state& q_I);
	vector<store_automaton> anonymize_by_split(const store_automaton& A);
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

	/// determine bar(R_k) = bar(R_{k+1})
	vector<config_top> extract_config_tops(const symbolic_config& tau);
	set<pda_alpha> extract_top_symbols(const store_automaton& A,
			const pda_state q);
	vector<vector<pda_alpha>> cross_product(const vector<set<pda_alpha>>& tops);
};

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following are the utilities for PDS file parser.
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
			const char& delim = '|');
	static concrete_config create_global_config_from_str(const string& s_ts,
			const char& delim = '|');

	static thread_state create_thread_state_from_str(const string& s_ts,
			const char& delim = '|');
	static global_state create_global_state_from_str(const string& s_ts,
			const char& delim = '|');

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
			const thread_state& final,  ///
			const vector<vector<vertex>>& mapping_Q, ///
			const vector<thread_state>& Q, ///
			const vector<pda_trans>& R);

	~simulator();

	void context_bounded_analysis(const size_t& n, const size_k& k);
private:
	///  Part 1: parse a pushdown system (PDS)
	ctx_bound k_bound;
	thread_state initl_TS;
	thread_state final_TS;
	vector<vector<vertex>> mapping_Q; /// mapping a control state to its ID
	vector<thread_state> active_Q;       /// active control states
	vector<pda_trans> active_R; /// active transitions
	adj_list PDS;

	vector<vector<bool>> reachable_T;

	uint bounded_reachability(const size_t& n, const size_k& k);

	antichain step(const concrete_config& tau);
	bool is_reachable(const concrete_config& tau, antichain& R);
	void marking(const pda_state& s, const pda_alpha& l);
	vertex retrieve(const pda_state& s, const pda_alpha& l);
};

} /* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
