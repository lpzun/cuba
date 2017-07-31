/**
 * @brief cfg.hh
 *
 * @date   Nov 16, 2015
 * @author Peizun Liu
 */

#ifndef UTIL_ALGS_HH_
#define UTIL_ALGS_HH_

#include <iostream>
#include <deque>
#include <bitset>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <stack>
#include <stdexcept>

using std::istream;
using std::ostream;
using std::cout;
using std::cerr;
using std::endl;

using std::string;
using std::bitset;
using std::vector;
using std::deque;
using std::map;
using std::unordered_map;
using std::stack;
using std::pair;

using std::runtime_error;

namespace bopp {

using symbol = short;
using size_pc = unsigned short;
using state_v = bitset<16>;

/**
 * @brief sool = symbolic boolean variables
 */
enum class sool {
	F = 0, T = 1, N = 2
};

inline sool operator !(const sool& v) {
    if (v == sool::F)
        return sool::T;
    if (v == sool::T)
        return sool::F;
    return sool::N;
}

/**
 * @brief sbool AND operator & The truth table is as follows:
 *			v1:	 F	F	T	N	F	T	T	N	N
 *			v2:	 F	T	F	F	N	T	N	T	N
 *			--------------------------------------
 *			re:	 F	F	F	F	F	T	N	N	N
 * @param s1
 * @param s2
 * @return see above table
 */
inline sool operator &(const sool& v1, const sool& v2) {
    if (v1 == sool::F || v2 == sool::F)
        return sool::F;
    if (v1 == sool::T && v2 == sool::T)
        return sool::T;
    return sool::N;
}

/**
 * @brief symbol OR operator | The truth table is as follows:
 *			v1:	 F	F	T	N	F	T	T	N	N
 *			v2:	 F	T	F	F	N	T	N	T	N
 *			--------------------------------------
 *			re:	 F	T	T	N	N	T	N	N	N
 * @param s1
 * @param s2
 * @return see above table
 */
inline sool operator |(const sool& s1, const sool& s2) {
    if (s1 == sool::N || s2 == sool::N)
        return sool::N;
    if (s1 == sool::F && s2 == sool::F)
        return sool::F;
    return sool::T;
}

/**
 * @brief overloading operator <<
 * @param out
 * @param s
 * @return output stream:
 *         print an edge in cfg
 */
inline ostream& operator <<(ostream& out, const sool& s) {
    switch (s) {
    case sool::F:
        out << 0;
        break;
    case sool::T:
        out << 1;
        break;
    case sool::N:
        out << 2;
        break;
    default:
        throw runtime_error("unknown value for Boolean variable");
        break;
    }
    return out;
}

/// symbolic valuation for shared variables
using symbval = vector<sool>;

class refs {
public:
	refs() {
	}
	~refs() {
	}

	static ushort SV_NUM;
	static ushort LV_NUM;
	static ushort PC_NUM;

	/// constant expression
	static const string CONST_N; /// constant nondeterminism
	static const string CONST_T; /// constant true
	static const string CONST_F; /// constant false

	///  unary expression
	static const string NEG; /// !, negation

	/// binary expression
	static const string AND; /// &, and
	static const string OR;  /// |, or
	static const string XOR; /// ^, exclusive or
	static const string EQ;  /// =, equal
	static const string NEQ; /// !=, not equal
	static const string IMPLIES; /// =>, implies

	/// parentheses "()"
	static const string PAREN_L;
	static const string PAREN_R;

	static const ushort omega;
};

/**
 * @brief solver this class contains all function to evaluate all
 *        possible Boolean expressions appearing in Boolean programs.
 */
class solver {
public:
	static const symbol CONST_F = 0; /// constant false
	static const symbol CONST_T = 1; /// constant true
	static const symbol CONST_N = 2; /// constant nondeterminism
	static const symbol NEG = -1; /// !, negation
	static const symbol AND = -2; /// &, and
	static const symbol OR = -3;  /// |, or
	static const symbol XOR = -4; /// ^, exclusive or
	static const symbol EQ = -5;  /// =, equal
	static const symbol NEQ = -6; /// !=, not equal
	static const symbol IMP = -7; /// =>, implies
	static const symbol PAR = -8; /// parenthesis

	solver();
	~solver();

	static bool solve(const deque<symbol>& sexpr, const state_v& s,
			const state_v& l);
	static bool solve(const deque<symbol>& sexpr, const state_v& s,
			const state_v& l, const state_v& _s, const state_v& _l);
	static deque<pair<symbval, symbval>> all_sat_solve(
			const deque<symbol>& sexpr, const symbval& s_vars,
			const symbval& l_vars);

	static deque<deque<symbol>> split(const deque<symbol>& sexpr);
	static deque<deque<sool>> split(const deque<sool>& vs);

	static void substitute(deque<symbol>& sexpr, const symbol& var,
			const symbol& ins);

	static symbol encode(const symbol& idx, const bool& is_shared);

	static string recov_expr_from_list(const deque<symbol>& sexpr);

private:
	static bool eval(const deque<symbol>& sexpr);

	static symbol decode(const symbol& idx, bool& is_shared);
	static symbol decode(const symbol& idx, bool& is_shared, bool& is_primed);
	static vector<bool> to_binary(const int& n, const short& shift);
	static int power(const int& base, const int& exponent);
};

} /* namespace bopp */

#endif /* UTIL_ALGS_HH_ */
