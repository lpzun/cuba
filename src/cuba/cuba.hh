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
class symbolic_cuba {
public:
	symbolic_cuba(const string& initl, const string& final,
			const string& filename, const size_t n = 0);
	~symbolic_cuba();

	void context_bounded_analysis(const size_k k = 0);

private:
	explicit_config initl_c;
	explicit_config final_c;
	concurrent_pushdown_automata CPDA;
	vector<set<top_config>> approx_X;

	/// Post*(A): build pushdown store automaton
	store_automaton create_store_automaton(const size_t i);
	store_automaton create_init_automaton(const size_t i);
	fsa_state create_accept_state(const fsa_state_set& states);
	fsa_state create_interm_state(const fsa_state_set& states);
	store_automaton post_kleene(const store_automaton& A,
			const pushdown_automaton& P);

	/// QR algorithm: context-bounded analysis
	bool context_bounded_analysis(const size_k k, const symbolic_config& cfg_I);
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
	bool converge(const vector<deque<symbolic_config>>& R, const size_k k,
			vector<set<top_config>>& top_R);
	bool is_convergent();
	uint top_mapping(const deque<symbolic_config>& R,
			vector<set<top_config>>& topped_R);
	vector<top_config> top_mapping(const symbolic_config& tau);
	set<pda_alpha> top_mapping(const store_automaton& A, const pda_state q);
	vector<vector<pda_alpha>> cross_product(const vector<set<pda_alpha>>& tops);
};

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following are the utilities for explore.
///
/////////////////////////////////////////////////////////////////////////

using antichain = deque<global_config>;
using finite_machine = map<thread_state, deque<transition<thread_state, thread_state>>>;
using concurrent_finite_machine = vector<finite_machine>;

/**
 * The class of explicit CUBA
 */
class explicit_cuba {
public:
	explicit_cuba(const string& initl, const string& final,
			const string& filename, const size_t n = 0);

	~explicit_cuba();

	void context_bounded_analysis(const size_k& k);
private:
	///  Part 1: parse a pushdown system (PDS)
	explicit_config initl_c;
	explicit_config final_c;
	concurrent_pushdown_automata CPDA;
	vector<set<top_config>> approx_X;
	vector<vector<bool>> reachable_T;

	bool k_bounded_reachability(const size_k k_bound,
			const explicit_config& c_I);
	antichain step(const global_config& tau, const bool is_switch);
	void step(const pda_state& q, const stack_vec& W, const uint tid,
			antichain& successors);
	bool update_R(vector<vector<antichain>>& R, const size_k k,
			const global_config& c);

	/// determine convergence, reachability of a target and so on
	bool converge(const vector<antichain>& R, const size_k k,
			vector<set<top_config>>& top_R);
	bool is_convergent();
	bool is_reachable(const global_config& tau, const size_k k,
			vector<vector<antichain>>& R);
	void marking(const pda_state& s, const pda_alpha& l);

	top_config top_mapping(const global_config& tau);

	/// cycle detection
	bool is_cyclic(const size_t tid);
	bool is_cyclic(const size_t tid, const thread_state& s, stack<pda_alpha>& W,
			map<vertex, bool>& visit, map<vertex, bool>& trace);
};

/////////////////////////////////////////////////////////////////////////
/// PART 3. A preprocessor, overapproximate the set of reachable top
/// configurations
///
/////////////////////////////////////////////////////////////////////////

class processor {
public:
	processor(const string& initl, const string& filename);
	processor(const explicit_config& initl, const string& filename);
	~processor();

	vector<set<top_config>> context_insensitive(const top_config& initl_c,
			const vector<finite_machine>& CFSM);

	const vector<set<top_config> >& get_approx_X() const {
		return approx_X;
	}

private:
	/// 1.1 <approx_X>: the overapproximation of the set of reachable
	///     popped top configurations
	vector<set<top_config>> approx_X;

	vector<set<top_config>> standard_FWS(const top_config& initl_c,
			const vector<finite_machine>& CFSM);
	deque<top_config> step(const top_config& c,
			const vector<finite_machine>& CFSM);
	top_config top_mapping(const explicit_config& tau);
	void print_approximation(const vector<set<top_config>>& approx_R);
};

/////////////////////////////////////////////////////////////////////////
/// PART 4. The following are the utilities for PDS file parser.
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
	static explicit_config create_global_config_from_str(const string& s_ts,
			const char& delim = prop::SHARED_LOCAL_DELIMITER);

	static thread_state create_thread_state_from_str(const string& s_ts,
			const char& delim = prop::SHARED_LOCAL_DELIMITER);
	static global_state create_global_state_from_str(const string& s_ts,
			const char& delim = prop::SHARED_LOCAL_DELIMITER);
};
}
/* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
