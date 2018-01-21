/**
 * @brief cuba.hh
 *
 * @date  : Aug 28, 2016
 * @author: Peizun Liu
 */

#ifndef CUBA_CUBA_HH_
#define CUBA_CUBA_HH_

#include "prop.hh"
#include "generator.hh"

using namespace ruba;

namespace cuba {

using antichain = deque<explicit_config_tid>;

/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the base class for context-unbounded
/// analysis.
/////////////////////////////////////////////////////////////////////////
class base_cuba {
public:
	base_cuba(const string& initl, const string& final, const string& filename,
			const size_n n);
	virtual ~base_cuba();
	virtual void context_unbounded_analysis(const size_k k_bound = 0) = 0;
protected:
	explicit_config initl_c;
	explicit_config final_c;
	concurrent_pushdown_automata CPDA;
	/// 1.1 <generators>: the overapproximation of the set of reachable
	///     popped top configurations
	vector<set<top_config>> generators;
	vector<vector<bool>> reachable_T;

private:
};

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following class is the symbolic version for
/// context-unbounded analysis.
/////////////////////////////////////////////////////////////////////////
class symbolic_cuba: public base_cuba {
public:
	symbolic_cuba(const string& initl, const string& final,
			const string& filename, const size_n n = 0);
	~symbolic_cuba();

	virtual void context_unbounded_analysis(const size_k k_bound = 0);
private:

	/// Post*(A): build pushdown store automaton
	store_automaton create_store_automaton(const size_n i);
	store_automaton create_init_automaton(const size_n i);
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
			const vector<store_automaton>& automata, const size_n& idx);
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
/// PART 3. The following class is the explicit version for
/// context-unbounded analysis.
/////////////////////////////////////////////////////////////////////////

/**
 * The class of explicit CUBA
 */
class explicit_cuba: public base_cuba {
public:
	explicit_cuba(const string& initl, const string& final,
			const string& filename, const size_n n = 0);

	virtual ~explicit_cuba();

	virtual void context_unbounded_analysis(const size_k k_bound = 0);
private:

	bool k_bounded_reachability(const size_k k_bound,
			const explicit_config& c_I);
	antichain step(const explicit_config_tid& tau, const bool is_switch);
	void step(const pda_state& q, const stack_vec& W, const uint tid,
			antichain& successors);
	bool update_R(vector<vector<antichain>>& R, const size_k k,
			const explicit_config_tid& c);

	/// determine convergence, reachability of a target and so on
	bool converge(const vector<antichain>& R, const size_k k,
			vector<set<top_config>>& top_R);
	bool is_convergent();
	bool is_reachable(const explicit_config_tid& tau, const size_k k,
			vector<vector<antichain>>& R);
	void marking(const pda_state& s, const pda_alpha& l);

	top_config top_mapping(const explicit_config_tid& tau);

	/// determine the finite context reachability
	bool finite_context_reachability(const size_n tid);
	bool finite_context_reachability(const size_n tid, const thread_visible_state& s,
			stack<pda_alpha>& W, map<vertex, bool>& visit,
			map<vertex, bool>& trace);
};
}
/* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
