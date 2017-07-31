/**
 * @brief algs.cc
 *
 * @date   Nov 17, 2015
 * @author Peizun Liu
 */

#include "algs.hh"

namespace bopp {

/// the number of shared variables
ushort refs::SV_NUM = 0;

/// the number of local  variables
ushort refs::LV_NUM = 0;

/// the number of program counters
ushort refs::PC_NUM = 0;

/// constant expression
const string refs::CONST_N = "*"; /// constant nondeterministic choice
const string refs::CONST_T = "1"; /// constant true
const string refs::CONST_F = "0"; /// constant false

/// unary operator
const string refs::NEG = "!";

/// binary operator
const string refs::AND = "&";
const string refs::OR = "|";
const string refs::XOR = "^";
const string refs::EQ = "=";
const string refs::NEQ = "!=";
const string refs::IMPLIES = "=>";

/// parentheses ()
const string refs::PAREN_L = "(";
const string refs::PAREN_R = ")";

const ushort refs::omega = std::numeric_limits<ushort>::max();

/**
 * @brief to restore the expression
 * @param symb_list
 * @param is_origi: this para is to generate the comments!!!!!!!
 * @return expression
 */
string solver::recov_expr_from_list(const deque<symbol>& sexpr) {
	stack<string> worklist;
	string op1, op2;
	for (const auto& ss : sexpr) {
		switch (ss) {
		case solver::AND:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 + " & " + op2);
			break;
		case solver::OR:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 + " | " + op2);
			break;
		case solver::XOR:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 + " ^ " + op2);
			break;
		case solver::EQ:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 + " = " + op2);
			break;
		case solver::NEQ:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 + " != " + op2);
			break;
		case solver::NEG:
			op1 = worklist.top(), worklist.pop();
			worklist.emplace("!" + op1);
			break;
		case solver::PAR:
			op1 = worklist.top(), worklist.pop();
			worklist.emplace("(" + op1 + ")");
			break;
		case solver::CONST_F:
			worklist.emplace("0");
			break;
		case solver::CONST_T:
			worklist.emplace("1");
			break;
		case solver::CONST_N:
			worklist.emplace("*");
			break;
		default:
			worklist.emplace(std::to_string(ss));
			break;
		}
	}
	return worklist.top();
}

///////////////////////////////////////////////////////////////////////////////
/// from here: class solver
///
///////////////////////////////////////////////////////////////////////////////

const short solver::CONST_F; /// constant false
const short solver::CONST_T; /// constant true
const short solver::CONST_N; /// constant nondeterminism
const short solver::NEG; /// !, negation
const short solver::AND; /// &, and
const short solver::OR;  /// |, or
const short solver::XOR; /// ^, exclusive or
const short solver::EQ;  /// =, equal
const short solver::NEQ; /// !=, not equal
const short solver::IMP; /// =>, implies
const short solver::PAR; /// parenthesis

solver::solver() {

}
solver::~solver() {

}

/**
 * @brief This is a customized "exhaustive" SAT solver, which can be used to
 *        extract targets from assertions in Boolean program. It's an
 *        exhaustive algorithm. I've no idea if we should use a more efficient
 *        SAT solver. It seems unnecessary due to that each assertion contains
 *        only very few boolean variables.
 * @param expr
 * @param s
 * @param l
 * @return a sat solver
 */
bool solver::solve(const deque<symbol>& sexpr, const state_v& s,
		const state_v& l) {
	stack<bool> worklist;
	bool op1, op2;
	for (const auto& ss : sexpr) {
		switch (ss) {
		case solver::AND:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 & op2);
			break;
		case solver::OR:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 | op2);
			break;
		case solver::XOR:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 ^ op2);
			break;
		case solver::EQ:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 == op2);
			break;
		case solver::NEQ:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 != op2);
			break;
		case solver::NEG:
			op1 = worklist.top(), worklist.pop();
			worklist.emplace(!op1);
			break;
		case solver::PAR:
			break;
		case solver::CONST_F:
			worklist.emplace(false);
			break;
		case solver::CONST_T:
			worklist.emplace(true);
			break;
		case solver::CONST_N:
			throw runtime_error("* is not splitted");
			break;
		default: {
			bool is_shared = false;
			const auto& i = solver::decode(ss, is_shared);
			if (is_shared)
				worklist.push(s[i]);
			else
				worklist.push(l[i]);
		}
			break;
		}
	}
	return worklist.top();
}

/**
 * @brief This is a customized "exhaustive" SAT solver, which can be used to
 *        extract targets from assertions in Boolean program. It's an
 *        exhaustive algorithm. I've no idea if we should use a more efficient
 *        SAT solver. It seems unnecessary due to that each assertion contains
 *        only very few boolean variables.
 * @param sexpr
 * @param s
 * @param l
 * @param _s
 * @param _l
 * @return
 */
bool solver::solve(const deque<symbol>& sexpr, const state_v& s,
		const state_v& l, const state_v& _s, const state_v& _l) {
	stack<bool> worklist;
	bool op1, op2;
	for (const auto& ss : sexpr) {
		switch (ss) {
		case solver::AND:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 & op2);
			break;
		case solver::OR:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 | op2);
			break;
		case solver::XOR:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 ^ op2);
			break;
		case solver::EQ:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 == op2);
			break;
		case solver::NEQ:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 != op2);
			break;
		case solver::NEG:
			op1 = worklist.top(), worklist.pop();
			worklist.emplace(!op1);
			break;
		case solver::PAR:
			break;
		case solver::CONST_F:
			worklist.emplace(false);
			break;
		case solver::CONST_T:
			worklist.emplace(true);
			break;
		case solver::CONST_N:
			throw runtime_error("* is not splitted");
			break;
		default: {
			bool is_shared = false, is_primed = false;
			const auto& i = solver::decode(ss, is_shared, is_primed);
			if (!is_primed) { /// normal variables
				if (is_shared)
					worklist.push(s[i]);
				else
					worklist.push(l[i]);
			} else { /// primed variables
				if (is_shared)
					worklist.push(_s[i]);
				else
					worklist.push(_l[i]);
			}
		}
			break;
		}
	}
	return worklist.top();
}

/**
 * @brief This is an all sat solver: it's an exhaustive one, so it's not
 *        very efficient.
 *        TODO: consider to call an off-the-shelf incremental SAT solver
 *
 * @param sexpr
 * @return all of the valuations that satisfy sexpr
 */
deque<pair<symbval, symbval>> solver::all_sat_solve(const deque<symbol>& sexpr,
		const symbval& s_vars, const symbval& l_vars) {
	cout << __func__ << recov_expr_from_list(sexpr) << "\n";

	deque<pair<symbval, symbval>> result;
	/// step 1: collect ids of free Boolean variables:
	///         this is a subset of all variables
	map<symbol, ushort> vfree_id;
	ushort vfree = 0; /// the number of free variables
	for (const auto& s : sexpr) {
		if (s > solver::CONST_N && vfree_id.emplace(s, vfree).second)
			++vfree;
	}
	/// if the expression does not involve any variable, then
	/// evaluate the expression, and based on the result:
	/// (1) true : all assignments satisfy the expression,
	/// (2) false: no assignment satisfies the expression.
	if (vfree == 0) {
		if (solver::eval(sexpr)) /// <expr> is true everywhere
			result.emplace_back(s_vars, l_vars);
		return result;
	}
	cout << __func__ << " " << vfree << endl;
	/// step 2: enumerate over all possible valuations
	///         of active Boolean variables
	for (auto assg = 0; assg < pow(2, vfree); ++assg) {
		/// step 3: build an assignment to active variables
		const auto& bv = solver::to_binary(assg, vfree);
		symbval s_tmp(s_vars);
		symbval l_tmp(l_vars);
		/// step 4: replace Boolean variables by its values
		auto se = sexpr;
		for (auto i = 0; i < se.size(); ++i) {
			if (se[i] > solver::CONST_N) {
				auto ifind = vfree_id.find(se[i]);
				/// step 4.1: determine whether se[i] is shared or not
				bool is_shared = false;
				const auto& idx = solver::decode(se[i], is_shared);
				if (is_shared) {
					s_tmp[idx] = (bv[ifind->second] ? sool::T : sool::F);
				} else {
					l_tmp[idx] = (bv[ifind->second] ? sool::T : sool::F);
				}
				/// step 4.2: assign a Boolean value to variable se[i]
				se[i] = bv[ifind->second];
			}
		}

		/// step 5: evaluate the expression
		if (solver::eval(se)) {
			result.emplace_back(s_tmp, l_tmp);
		}
	}
#ifndef NDEBUG
	cout << __func__ << "====================\n";
	for (const auto& p : result) {
		for (const auto& s : p.first)
			cout << s;
		for (const auto& l : p.second)
			cout << l;
		cout << endl;
	}
#endif
	return result;
}

/**
 * @brief split * into 0 and 1 during expression evaluation
 * @param sexpr: a expression with *
 * @return a list of expressions after splitting * into 0 and 1
 */
deque<deque<symbol>> solver::split(const deque<symbol>& sexpr) {
	deque<deque<symbol>> worklist;
	worklist.emplace_back(sexpr);
	deque<deque<symbol>> splitted;
	for (auto i = 0; i < sexpr.size(); ++i) {
		if (sexpr[i] == solver::CONST_N) {
			while (!worklist.empty()) {
				auto e1 = worklist.front();
				worklist.pop_front();
				auto e2 = e1;

				e1[i] = solver::CONST_F;
				e2[i] = solver::CONST_T;

				splitted.emplace_back(e1);
				splitted.emplace_back(e2);
			}
			splitted.swap(worklist);
		}
	}
	return worklist;
}

/**
 * @brief do substitution:
 * @param sexpr:
 * @param var  :
 * @param ins  :
 */
void solver::substitute(deque<symbol>& sexpr, const symbol& var,
		const symbol& ins) {
	for (auto& s : sexpr) {
		if (s == var)
			s = ins;
	}
}

/**
 * @brief split * into 0 and 1 during expression evaluation
 * @param sexpr: a expression with *
 * @return a list of expressions after splitting * into 0 and 1
 */
deque<deque<sool>> solver::split(const deque<sool>& vsool) {
	deque<deque<sool>> worklist;
	worklist.emplace_back(vsool);
	deque<deque<sool>> splitted;
	for (auto i = 0; i < vsool.size(); ++i) {
		if (vsool[i] == sool::N) {
			while (!worklist.empty()) {
				auto v1 = worklist.front();
				worklist.pop_front();
				auto v2 = v1;

				v1[i] = sool::F;
				v2[i] = sool::T;

				splitted.emplace_back(v1);
				splitted.emplace_back(v2);
			}
			splitted.swap(worklist);
		}
	}
	return worklist;
}

symbol solver::encode(const symbol& idx, const bool& is_shared) {
	if (is_shared)
		return idx + 3;
	else
		return idx + 3 + refs::SV_NUM;
}

/**
 * @brief decode
 * @param idx
 * @param is_shared
 */
symbol solver::decode(const symbol& idx, bool& is_shared) {
	auto id = idx - 3;
	is_shared = true;
	if (id >= refs::SV_NUM)
		is_shared = false, id -= refs::SV_NUM;
	return id;
}

/**
 * @brief decode
 * @param idx
 * @param is_shared
 * @param is_primed
 */
symbol solver::decode(const symbol& idx, bool& is_shared, bool& is_primed) {
	auto id = idx - 3;
	if (id < refs::SV_NUM + refs::LV_NUM) {
		is_primed = false, is_shared = true;
		if (id >= refs::SV_NUM)
			is_shared = false, id -= refs::SV_NUM;
	} else {
		is_primed = true, is_shared = true;
		if (id >= 2 * refs::SV_NUM + refs::LV_NUM)
			is_shared = false, id -= refs::SV_NUM;
		id = id - refs::SV_NUM - refs::LV_NUM;
	}
	return id;
}

/**
 * @brief convert a unsigned to a binary representation
 * @param n
 * @param bits
 * @return a binary stored in vector<bool>
 */
vector<bool> solver::to_binary(const int& n, const short& shift) {
	auto num = n;
	vector<bool> bv(shift);
	for (auto i = 0; i < shift; ++i) {
		int b = (num >> i) & 1;
		bv[i] = b == 1 ? 1 : 0;
	}
	return bv;
}

/**
 * @brief to computer power(base, exponent)
 * @param base
 * @param bits
 * @return
 */
int solver::power(const int& base, const int& exponent) {
	auto shift = exponent;
	int result = 1;
	while (shift-- > 0) {
		result *= base;
	}
	return result;
}

/**
 * @brief to evaluate a Boolean expression whose variables have
 *        already been replaced by their valuations.
 * @param sexpr: a Boolean expression
 * @return evaluation result: true or false
 */
bool solver::eval(const deque<symbol>& sexpr) {
	stack<bool> worklist;
	bool op1, op2;
	for (const auto& ss : sexpr) {
		switch (ss) {
		case solver::AND:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 & op2);
			break;
		case solver::OR:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 | op2);
			break;
		case solver::XOR:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 ^ op2);
			break;
		case solver::EQ:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 == op2);
			break;
		case solver::NEQ:
			op1 = worklist.top(), worklist.pop();
			op2 = worklist.top(), worklist.pop();
			worklist.emplace(op1 != op2);
			break;
		case solver::NEG:
			op1 = worklist.top(), worklist.pop();
			worklist.emplace(!op1);
			break;
		case solver::PAR:
			break;
		case solver::CONST_F:
			worklist.emplace(false);
			break;
		case solver::CONST_T:
			worklist.emplace(true);
			break;
		case solver::CONST_N:
			throw runtime_error("* is not splitted");
			break;
		default:
			throw("A variable appears in solver::eval()");
			break;
		}
	}
	if (worklist.empty())
		return false;
	return worklist.top();
}

} /* namespace bopp */
