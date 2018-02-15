/**
 * Write-(Un)bounded Analysis
 *
 * @date   Jan 1, 2018
 * @author TODO
 */

#ifndef WUBA_WUBA_HH_
#define WUBA_WUBA_HH_

#include "prop.hh"
#include "generator.hh"

using namespace ruba;

namespace wuba {
/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the base class for write-unbounded
/// analysis.
/////////////////////////////////////////////////////////////////////////
class base_wuba {
public:
	base_wuba(const string& initl, const string& final, const string& filename);
	virtual ~base_wuba();

	virtual void write_bounded_analysis(const size_k k_bound) = 0;
	virtual void write_unbounded_analysis() = 0;

protected:
	explicit_state initl_c;
	explicit_state final_c;
	concurrent_pushdown_automata CPDA;
	/// 1.1 <generators>: the overapproximation of the set of reachable
	///     popped top configurations
	vector<set<visible_state>> generators;
	vector<vector<bool>> reachable_T;

	size_k convergence_GS; /// convergence of OS of global states
	size_k convergence_VS; /// convergence of OS of visible states
};

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following class is the explicit version for
/// write-unbounded analysis.
/////////////////////////////////////////////////////////////////////////
class explicit_wuba: public base_wuba {
public:
	explicit_wuba(const string& initl, const string& final,
			const string& filename);
	virtual ~explicit_wuba();

	virtual void write_bounded_analysis(const size_k k_bound);
	virtual void write_unbounded_analysis();

private:
	bool k_bounded_reachability(const size_k k_bound,
			const explicit_state& c_I);
	deque<explicit_state> k_round(const deque<explicit_state>& R_k);
	deque<explicit_state> step(const explicit_state& tau);
	bool update_R(const explicit_state& tau, const size_k k,
			vector<vector<deque<explicit_state>>>& R);
	bool is_reachable(const explicit_state& tau, const size_k k,
			vector<vector<deque<explicit_state>>>& R);

	bool converge(const vector<deque<explicit_state>>& Rk, const size_k k,
			vector<set<visible_state>>& top_R);
	bool converge();
	visible_state top_mapping(const explicit_state& tau);
};

/////////////////////////////////////////////////////////////////////////
/// PART 3. The following class is the symbolic version for
/// write-unbounded analysis.
/////////////////////////////////////////////////////////////////////////
class symbolic_wuba: public base_wuba {
public:
	symbolic_wuba(const string& initl, const string& final,
			const string& filename);
	virtual ~symbolic_wuba();

	virtual void write_bounded_analysis(const size_k k_bound);
	virtual void write_unbounded_analysis();

private:

};

} /* namespace wuba */

#endif /* WUBA_WUBA_HH_ */
