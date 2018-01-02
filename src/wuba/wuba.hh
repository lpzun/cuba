/**
 * wuba.hh
 *
 * @date  : Jan 1, 2018
 * @author: lpzun
 */

#ifndef WUBA_WUBA_HH_
#define WUBA_WUBA_HH_

#include "prop.hh"
#include "parsers.hh"

using namespace ruba;

namespace wuba {
/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the base class for write-unbounded
/// analysis.
/////////////////////////////////////////////////////////////////////////
class base_wuba {
public:
	base_wuba(const string& initl, const string& final, const string& filename,
			const size_n n_bound);
	virtual ~base_wuba();

	virtual void write_bounded_analysis(const size_k k_bound) = 0;
	virtual void write_unbounded_analysis() = 0;

protected:
	size_n n_bound; /// the number of threads, only for parameterized programs
	explicit_config initl_c;
	explicit_config final_c;
	concurrent_pushdown_automata CPDA;
};

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following class is the explicit version for
/// write-unbounded analysis.
/////////////////////////////////////////////////////////////////////////
class explicit_wuba: public base_wuba {
public:
	explicit_wuba(const string& initl, const string& final,
			const string& filename, const size_n n_bound = 0);
	virtual ~explicit_wuba();

	virtual void write_bounded_analysis(const size_k k_bound);
	virtual void write_unbounded_analysis();

private:
	bool k_bounded_reachability(const size_k k_bound,
			const explicit_config& c_I);
	deque<explicit_config> k_round(const deque<explicit_config>& R_k);
	deque<explicit_config> step(const explicit_config& c);
	bool update_R(vector<vector<deque<explicit_config>>>& global_R,
			const size_k k, const explicit_config& c);
};

/////////////////////////////////////////////////////////////////////////
/// PART 3. The following class is the symbolic version for
/// write-unbounded analysis.
/////////////////////////////////////////////////////////////////////////
class symbolic_wuba: public base_wuba {
public:

private:

};

} /* namespace wuba */

#endif /* WUBA_WUBA_HH_ */
