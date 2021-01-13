/**
 * cuba.hh
 *
 * @date   Aug 28, 2016
 * @author TODO
 */

#ifndef CUBA_CUBA_HH_
#define CUBA_CUBA_HH_

#include <flags.hh>
#include "generator.hh"

using namespace ruba;

namespace cuba {

/////////////////////////////////////////////////////////////////////////
/// base_cuba: the base class for context-unbounded analysis. Classes
/// symbolic_cuba and explicit_cuba are inherited from it.
/////////////////////////////////////////////////////////////////////////
class base_cuba {
public:
	base_cuba(const string& initl, const string& final, const string& filename);
	virtual ~base_cuba();
	virtual void context_unbounded_analysis(const size_k k_bound = 0) = 0;

protected:
	bool reachable;
	/// explicit initial state
	explicit_state initl_c;
	/// explicit target state
	visible_state final_c;
	/// concurrent pushdown system
	concurrent_pushdown_automata CPDA;
	/// generators: used for determining the convergence
	vector<set<visible_state>> generators;
	/// used for marking the set of reachable thread states, only supporting
	/// parameterized system for now
	vector<vector<bool>> reachable_T;

	// file dump
	string filename_global_R;
	string filename_top_R;

private:
	visible_state top_mapping(const explicit_state& tau);
};

/////////////////////////////////////////////////////////////////////////
/// symbolic_cuba: A symbolic version for context-unbounded analysis.
/////////////////////////////////////////////////////////////////////////
class symbolic_cuba: public base_cuba {
public:
	symbolic_cuba(const string& initl, const string& final,
			const string& filename);
	~symbolic_cuba();

	virtual void context_unbounded_analysis(const size_k k_bound = 0);
private:

	/// Post*(A): build pushdown store automaton
	store_automaton create_init_automaton(const pushdown_automaton& P,
			const pda_state q_I, const pda_stack& w);
	store_automaton post_kleene(const store_automaton& A,
			const pushdown_automaton& P);

	/// QR algorithm: context-bounded analysis
	bool context_bounded_analysis(const size_k k, const symbolic_state& cfg_I);
	set<fsa_state> project_Q(const store_automaton& A);
	set<fsa_state> BFS_visit(const fsa_state& root,
			const unordered_map<fsa_state, deque<fsa_state>>& adj,
			const fsa_state_set& initials);
	symbolic_state compose(const pda_state& _q, const store_automaton& Ai,
			const vector<store_automaton>& automata, const size_n& idx);
	store_automaton rename(const store_automaton& A, const pda_state& q_I);
	store_automaton anonymize(const store_automaton& A, const pda_state& q_I,
			const bool& is_rename = false);
	store_automaton anonymize_by_split(const store_automaton& A,
			const pda_state& q_I);
	store_automaton anonymize_by_rename(const store_automaton& A,
			const pda_state& q_I);

	/// determine R_k = R_{k+1}
	bool is_recongnizable(const store_automaton& A, const thread_state& c);
	bool is_equivalent(const store_automaton& A1, const store_automaton& A2);
	store_automaton iunion(const store_automaton& A1,
			const store_automaton& A2);
	store_automaton intersect(const store_automaton& A1,
			const store_automaton& A2);
	store_automaton complement(const store_automaton& A);
	store_automaton cross_product(const vector<store_automaton>& W);

	/// determine bar(R_k) = bar(R_{k+1})s
	bool converge(const vector<deque<symbolic_state>>& R, const size_k k,
			vector<set<visible_state>>& top_R);
	bool is_convergent();
	uint top_mapping(const deque<symbolic_state>& R,
			vector<set<visible_state>>& topped_R);
	vector<visible_state> top_mapping(const symbolic_state& tau);
	set<pda_alpha> top_mapping(const store_automaton& A, const pda_state q);
	vector<vector<pda_alpha>> cross_product(const vector<set<pda_alpha>>& tops);
};

/// To store unordering explicit states
using antichain = deque<explicit_state_tid>;

/////////////////////////////////////////////////////////////////////////
/// explicit_cuba: An explicit version for context-unbounded analysis.
/////////////////////////////////////////////////////////////////////////
class explicit_cuba: public base_cuba {
public:
	explicit_cuba(const string& initl, const string& final,
			const string& filename);

	virtual ~explicit_cuba();

	virtual void context_unbounded_analysis(const size_k k_bound = 0);

	uint get_number_of_image_calls() const;
private:
	bool k_bounded_reachability(const size_k k_bound,
			const explicit_state& c_I);
	antichain step(const explicit_state_tid& tau, const bool is_switch);
	void step(const pda_state& q, const stack_vec& W, const uint tid,
			antichain& successors);
	bool update_R(vector<vector<antichain>>& R, const size_k k,
			const explicit_state_tid& c);

	/// determine convergence, reachability of a target and so on
	bool converge(const vector<vector<antichain>>& R, const size_k k,
			vector<set<visible_state>>& top_R, const bool next_level);
	uint update_top_R(const vector<vector<antichain>>& R, const size_k k,
			vector<set<visible_state>>& top_R);
	bool is_convergent();
	bool is_reachable(const explicit_state_tid& tau, const size_k k,
			vector<vector<antichain>>& R);
	void marking(const pda_state& s, const pda_alpha& l);

	visible_state top_mapping(const explicit_state_tid& tau);

	/// determine the finite context reachability
	bool finite_context_reachability(const size_n tid);
	bool finite_context_reachability(const pushdown_automaton& PDA,
			const thread_visible_state& s, stack<pda_alpha>& W,
			map<thread_visible_state, bool>& visit,
			map<thread_visible_state, bool>& trace);

	void dump_metrics(const vector<vector<antichain>>& global_R,
			const vector<set<visible_state>>& top_R) const;

	uint number_of_image_calls;
};
}
/* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
