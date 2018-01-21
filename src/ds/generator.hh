/**
 * generator.hh
 *
 * @date  : Jan 19, 2018
 * @author: Peizun Liu
 */

#ifndef DS_GENERATOR_HH_
#define DS_GENERATOR_HH_

#include "parsers.hh"

namespace ruba {

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following are for the generator idea
///
/////////////////////////////////////////////////////////////////////////
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
	top_config top_mapping(const explicit_state& tau);
	void print_approximation(const vector<set<top_config>>& approx_R);
};

} /* namespace ruba */

#endif /* DS_GENERATOR_HH_ */
