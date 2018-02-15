/**
 * generator.hh
 *
 * @date  : Jan 19, 2018
 * @author: TODO
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

	const vector<set<visible_state> >& get_generators() const {
		return generators;
	}

private:
	vector<set<visible_state>> generators;

	void context_insensitive(const string& initl, const string& filename);

	void context_insensitive(const visible_state& initl, const string& filename);
	vector<set<visible_state>> context_insensitive(const visible_state& initl_c,
			const vector<finite_machine>& CFSM);
	vector<set<visible_state>> standard_FWS(const visible_state& initl_c,
			const vector<finite_machine>& CFSM);
	deque<visible_state> step(const visible_state& c,
			const vector<finite_machine>& CFSM);
	visible_state top_mapping(const explicit_state& tau);
	void print_approximation(const vector<set<visible_state>>& approx_R);
};

} /* namespace ruba */

#endif /* DS_GENERATOR_HH_ */
