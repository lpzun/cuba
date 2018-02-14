/**
 * @brief bopp.cc
 *
 * @date  : Jan 12, 2016
 * @author: lpzun
 */

#include "aide.hh"

namespace ijit {

/**
 * @brief default constructor
 * @param m
 */
paide::paide(const mode& m) :
		lineno(0), ipc(0), s_vars_num(0), l_vars_num(0), s_vars_list(),  ///
		l_vars_list(), s_vars_init(), l_vars_init(), suc_pc_set(),       ///
		expr_in_list(), assg_stmt_lhs(), assg_stmt_rhs(), asse_pc_set(), ///
		cfg_G(), all_pc_set(), m(m) {
}

/**
 * @brief default destructor
 */
paide::~paide() {
}

/**
 * @brief add variables and initialization
 * @param var
 * @param val
 * @param is_shared
 */
void paide::add_vars(const string& var, const sool& val,
		const bool& is_shared) {
	if (is_shared) {
		const auto& p = s_vars_list.emplace(var, s_vars_num + 1);
		if (p.second)
			s_vars_init.emplace(++s_vars_num, val);
	} else {
		const auto& p = l_vars_list.emplace(var, l_vars_num + 1);
		if (p.second)
			l_vars_init.emplace(++l_vars_num, val);
	}
}

/**
 * @brief recorde the initialization of boolean variables
 * @param var
 * @param val
 */
void paide::init_vars(const string& var, const sool& val) {
	map<string, ushort>::iterator ifind;
	if ((ifind = s_vars_list.find(var)) != s_vars_list.end())
		s_vars_init[ifind->second] = val;
	else if ((ifind = l_vars_list.find(var)) != l_vars_list.end())
		l_vars_init[ifind->second] = val;
	else
		throw ijit_runtime_error("No such a variable named " + var);
}

/**
 * @brief to determine if the pc is unique or not.
 *        This is a testing function. I will be removed in the future.
 * @param pc
 */
bool paide::is_pc_unique(const size_pc& pc) {
	const auto& result = all_pc_set.emplace(pc);
	if (!result.second)
		throw ijit_runtime_error(
				"Parser: pc " + std::to_string(pc) + " duplicates.");
	return true;
}

/**
 * @brief to create the edge for goto or non-deterministic statements
 * @param from
 * @param sp
 */
void paide::add_edge(const size_pc& src, const type_stmt& type) {
	if (m == mode::POST) {
		for (const auto& dest : suc_pc_set)
			cfg_G.add_edge(src, dest, type);
	} else if (m == mode::PREV) {
		for (const auto& dest : suc_pc_set)
			cfg_G.add_edge(dest, src, type);
	}
}

/**
 * @brief to add an edge to a control flow graph
 * @param from
 * @param to
 * @param type
 * @param is_condition
 */
void paide::add_edge(const size_pc& src, const size_pc& dest,
		const type_stmt& type, const bool& is_condition) {
	/// step 1: add edges to non-goto statements
	if (m == mode::POST) { /// postimage mode
		if (!is_condition)
			cfg_G.add_edge(src, dest, type);
		else
			cfg_G.add_edge(src, dest, type, expr(expr_in_list));
	} else if (m == mode::PREV) { /// preimage mode
		if (!is_condition) {
			cfg_G.add_edge(dest, src, type);
		} else {
			cfg_G.add_edge(dest, src, type, expr(expr_in_list));
			/// add the <else> branch into edges collection
			if (type == type_stmt::IFEL)
				cfg_G.add_edge(src + 1, src, type, expr(expr_in_list));
		}
	}

	/// step 2: build expressions for parallel assignment
	if (type == type_stmt::ASSG) {
		cfg_G.add_assignment(src, create_assignment());
	}
}

/**
 * @brief build an assignment
 * @return the assignment that corresponds to the current statement
 */
assignment paide::create_assignment() {
	assignment assg(this->s_vars_num, this->l_vars_num);
	auto il = assg_stmt_lhs.cbegin(), iend = assg_stmt_lhs.cend();
	auto ir = assg_stmt_rhs.cbegin(), eend = assg_stmt_rhs.cend();
	while (il != iend && ir != eend) {
		const auto& p = this->decode(*il);
		if (p.second)
			assg.sh[p.first] = expr(*ir);
		else
			assg.lo[p.first] = expr(*ir);
		++il, ++ir;
	}
	return assg;
}

/**
 * @brief look up the index of iden in the map of variables
 * @param iden
 * @return index if find var
 *         throw an exception otherwise
 */
symbol paide::encode(const string& var) {
	symbol idx = 2;
	if (var.front() != '\'') {
		auto ifind = s_vars_list.find(var);
		if (ifind == s_vars_list.end()) {
			ifind = l_vars_list.find(var);
			if (ifind != l_vars_list.end())
				idx += s_vars_num;
			else
				throw ijit_runtime_error(var + "is missed!");
		}
		idx += ifind->second;
	} else {
		auto ifind = s_vars_list.find(var.substr(1));
		if (ifind == s_vars_list.end()) {
			ifind = l_vars_list.find(var.substr(1));
			if (ifind != l_vars_list.end())
				idx += s_vars_num;
			else
				throw ijit_runtime_error(var + "is missed!");
		}
		idx += ifind->second + s_vars_num + l_vars_num;
	}
	return idx;
}

/**
 * @brief decode the symbol
 * @param idx
 * @return pair<symbol, bool>
 *         symbol: the symbol of Boolean variables
 *         bool  : true if shared Boolean variable
 *                 false if local Boolean variable
 */
pair<symbol, bool> paide::decode(const symbol& idx) {
	auto id = idx - 3;
	if (id < s_vars_num)
		return std::make_pair(id, true);
	else
		return std::make_pair(id - s_vars_num, false);
}

/////////////////////////////////////////////////
///  unit test
/////////////////////////////////////////////////
/**
 * @brief output the control flow graph to the file
 * @param file
 */
void paide::print_control_flow_graph() {
	cout << cfg_G << endl;
}

/**
 * @brief testing methods
 */
void paide::print_parallel_assg_stmt() {
	auto i_iter = assg_stmt_lhs.begin(), i_end = assg_stmt_lhs.end();
	auto e_iter = assg_stmt_rhs.begin(), e_end = assg_stmt_rhs.end();
	while (i_iter != i_end && e_iter != e_end) {
		cout << *i_iter << ":=" << solver::recov_expr_from_list(*e_iter) << "\n";
		i_iter++, e_iter++;
	}
}

/**
 * @brief print vars list
 */
void paide::print_vars_list() {
	for (const auto& p : s_vars_list)
		cout << p.first << " " << p.second << " " << encode(p.first) << "\n";
	for (const auto& p : l_vars_list)
		cout << p.first << " " << p.second << " " << encode(p.first) << "\n";
}

} /* namespace iotf */
