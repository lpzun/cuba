/**
 * generator.cc
 *
 * @date  : Jan 19, 2018
 * @author: Peizun Liu
 */

#include <generator.hh>

namespace ruba {

/////////////////////////////////////////////////////////////////////////
/// PART 2. A preprocessor, overapproximate the set of reachable top
/// configurations
///
/////////////////////////////////////////////////////////////////////////
/**
 * Default constructor
 * @param initl   : initial configuration
 * @param filename: input CPDS
 */
generator::generator(const string& initl,
		const concurrent_pushdown_automata& CPDA,
		const concurrent_finite_machine& CFSM) :
		generators_for_fixed_bound_1(
				vector<set<visible_state>>(thread_visible_state::S)), approx_Z(
				vector<set<visible_state>>(thread_visible_state::S)), generators_for_dynamic_bound(
				vector<set<visible_state>>(thread_visible_state::S)), initl_tau(
				parser::parse_input_cfg(initl)), CPDA(CPDA), CFSM(CFSM), number_of_image_calls(
				0) {
	context_insensitive();
}

generator::~generator() {

}

/**
 * A constructor with initial top configuration and a concurrent finite machine
 *
 * @param initl   : initial configuration
 * @param filename: the file name for CPDS
 */
void generator::context_insensitive() {
	cout << logger::MSG_SEPARATOR << "compute generators...\n";
	context_insensitive_with_dynamic_bound();
	context_insensitive_with_fixed_bound_1();

	int size_generators_for_fixed_bound_1 = 0;
	int size_generators_for_dynamic_bound = 0;
	for (int i = 0; i < generators_for_fixed_bound_1.size(); ++i) {
		int fixed_1_size = generators_for_fixed_bound_1[i].size();
		int dynamic_size = generators_for_dynamic_bound[i].size();
		size_generators_for_fixed_bound_1 += fixed_1_size;
		size_generators_for_dynamic_bound += dynamic_size;
		cout << fixed_1_size << " vs " << dynamic_size << endl;
//		if (fixed_1_size != dynamic_size) {
//			cout<<"generators diffs: \n";
//			for (const auto s: generators_for_fixed_bound_1[i]) {
//				auto ifind = generators_for_dynamic_bound[i].find(s);
//				if (ifind == generators_for_dynamic_bound[i].end())
//					cout << s << "\n";
//			}
//			for (const auto s : generators_for_dynamic_bound[i]) {
//				auto ifind = generators_for_fixed_bound_1[i].find(s);
//				if (ifind == generators_for_fixed_bound_1[i].end())
//					cout << s << "\n";
//			}
//		}
	}
	cout << "size_generators_for_fixed_bound_1 = "
			<< size_generators_for_fixed_bound_1 << "\n"
			<< "size_generators_for_dynamic_bound = "
			<< size_generators_for_dynamic_bound << "\n";

	if (flags::OPT_PRINT_ALL) {
		cout << "Approximation Z:\n";
		this->print_approximation(approx_Z);
		cout << "G intersects Z:\n";
		this->print_approximation(get_generators());
	}
	cout << "The number of image calls in computing generators: "
			<< get_number_of_image_calls() << "\n";
	cout << logger::MSG_SEPARATOR;
}

/**
 *
 * @param stack_bound_for_Z
 */
void generator::context_insensitive_with_dynamic_bound() {
	dynamic_bound_standard_FWS();
}

void generator::dynamic_bound_standard_FWS() {
	vector<deque<explicit_state>> bounded_stack_global_R(
			thread_visible_state::S, deque<explicit_state>());
	queue<explicit_state> worklist( { initl_tau });
	while (!worklist.empty()) {
		const auto tau = worklist.front();
		worklist.pop();

		if (!update_R(tau, bounded_stack_global_R)) {
			continue;
		}
		for (const auto& _tau : step(tau)) {
			worklist.push(_tau);
		}
	}
}

deque<explicit_state> generator::step(const explicit_state& tau) {
	deque<explicit_state> successors;
	const auto& q = tau.get_state();
	const auto& W = tau.get_stacks();

	for (size_t tid = 0; tid < W.size(); ++tid) {
		step(q, W, tid, successors);
	}
	return successors;
}

void generator::step(const pda_state& q, const stack_vec& W, const uint tid,
		deque<explicit_state>& successors) {
	if (tid >= CPDA.size() || W[tid].empty()) {
		return;
	}

	const thread_visible_state src(q, W[tid].top());
	const auto ifind = CPDA[tid].get_program().find(src);
	if (ifind == CPDA[tid].get_program().end())
		return;

	for (const auto rid : ifind->second) { /// rid: transition id
		const auto& r = CPDA[tid].get_actions()[rid];
		const auto& dst = r.get_dst();

		auto _W = W; /// duplicate the stacks in current global conf.

		switch (r.get_oper_type()) {
		case type_stack_operation::PUSH: { /// push operation
			_W[tid].pop();
			for (auto is = dst.get_stack().get_worklist().rbegin();
					is != dst.get_stack().get_worklist().rend(); ++is) {
				_W[tid].push(*is);
			}
			if (_W[tid].size() == flags::OPT_Z_APPROXIMATION_BOUND + 1) {
				_W[tid].pop_back();
			}
			successors.emplace_back(dst.get_state(), _W);
		}
			break;
		case type_stack_operation::POP: { /// pop operation
			if (_W[tid].pop()) {
				const explicit_state successor(dst.get_state(), _W);
				successors.push_back(successor);
				generators_for_dynamic_bound[dst.get_state()].insert(
						top_mapping(successor));
			}
			if (_W[tid].size() == flags::OPT_Z_APPROXIMATION_BOUND - 1) {
				for (const auto alpha : parser::pop_candiate_sets[tid]) {
					_W[tid].push_back(alpha);
					const explicit_state successor(dst.get_state(), _W);
					successors.push_back(successor);
					generators_for_dynamic_bound[dst.get_state()].insert(
							top_mapping(successor));
					_W[tid].pop_back(); /// Recover the stack
				}
			}
		}
			break;
		default: { /// overwrite operation
			if (_W[tid].overwrite(dst.get_stack().top())) {
				successors.emplace_back(dst.get_state(), _W);
			}
		}
			break;
		}
	}
	++number_of_image_calls;
}

bool generator::update_R(const explicit_state& tau,
		vector<deque<explicit_state>>& R) {
	const auto q = tau.get_state();
	for (const auto& reached_tau : R[q]) {
		if (tau == reached_tau) {
			return false;
		}
	}
	R[q].push_back(tau);
	return true;
}

/**
 *
 * @param top_initl
 * @param CFSM
 * @return
 */
void generator::context_insensitive_with_fixed_bound_1() {
	/// Print Concurrent FSM
	if (flags::OPT_PRINT_ADJ) {
		for (const auto& fsm : CFSM) {
			for (const auto& adjlist : fsm)
				for (const auto& r : adjlist.second)
					cout << r << "\n";
		}
	}
	fixed_bound_standard_FWS();
}

/**
 * Standard forward search: to compute the overapproximation of the set of
 * reachable top configurations.
 *
 * @return a set of reachable top configurations
 */
vector<set<visible_state>> generator::fixed_bound_standard_FWS() {
	/// step 1: set up the data structures
	/// 1.1 <worklist>: a worklist
	queue<visible_state> worklist( { top_mapping(initl_tau) });
	while (!worklist.empty()) {
		const auto c = worklist.front();
		worklist.pop();
		const auto& ret = approx_Z[c.get_state()].emplace(c);
		if (!ret.second)
			continue;
		for (const auto& _c : step(c))
			worklist.emplace(_c);
	}
	return approx_Z;
}

/**
 * Compute the successors of top configuration c and update approx_X
 * @param c
 * @return return a list of top configurations
 */
deque<visible_state> generator::step(const visible_state& c) {
	deque<visible_state> successors;
	for (uint i = 0; i < c.get_local().size(); ++i) {
		auto ifind = CFSM[i].find(
				thread_visible_state(c.get_state(), c.get_local()[i]));
		if (ifind == CFSM[i].end())
			continue;
		for (const auto& trans : ifind->second) {
			/// set up successors' control states
			auto _q = trans.get_dst().get_state();
			/// set up successors' stack contents
			auto _local(c.get_local());
			_local[i] = trans.get_dst().get_alpha();
			successors.emplace_back(_q, _local);
			if (trans.get_oper_type() == type_stack_operation::POP) {
				generators_for_fixed_bound_1[_q].emplace(_q, _local);
			}
		}
	}
	return successors;
}

/**
 * Obtain the top of configuration
 * @param tau
 * @return
 */
visible_state generator::top_mapping(const explicit_state& tau) {
	vector<pda_alpha> W(tau.get_stacks().size());
	for (size_n i = 0; i < tau.get_stacks().size(); ++i) {
		if (tau.get_stacks()[i].empty())
			W[i] = alphabet::EPSILON;
		else
			W[i] = tau.get_stacks()[i].top();
	}
	return visible_state(tau.get_state(), W);
}

/**
 * Print approximation
 */
void generator::print_approximation(
		const vector<set<visible_state>>& approx_R) const {
	for (const auto& v : approx_R) {
		if (v.size() == 0)
			continue;
		cout << "  ";
		for (const auto& c : v)
			cout << c << " ";
		cout << "\n";
	}
}

uint generator::get_number_of_image_calls() const {
	return number_of_image_calls;
}

} /* namespace ruba */
