/**
 * @brief cfg.hh: the header file of some common data structures:
 *        type_stmt: the type of statements
 *        expr     : expression used in Boolean programs
 *        stmt     : it wraps the type and condition (if applicable)
 *                   of a statement.
 *        edge     : the edge of CFG; it wraps the source \pc and
 *                   destination \pc, as well as the statement.
 *
 * @date   Nov 17, 2015
 * @author Peizun Liu
 */

#ifndef API_CFG_HH_
#define API_CFG_HH_

#include "algs.hh"

namespace bopp {

/**
 * @brief data structure <type_stmt>: all of statement types
 *        SKIP = -1 : all other statement
 *        GOTO = -2 : goto statement
 *        ASSG = -3 : assignment statement
 *        IFEL = -4 : if (<expr>) then <sseq> else <sseq>; fi;
 *        ASSE = -5 : assertion statement
 *        ASSU = -6 : assume statement
 *        NTHR = -7 : thread creation statement
 *        ETHR = -8 : thread termination statement
 *        ATOM = -9 : atomic section beginning
 *        EATM = -10: atomic section ending
 *        BCST = -11: broadcast statement
 *        WAIT = -12: wait statement
 *        SIGN = -13: signal statement
 */
enum class type_stmt {
	SKIP = 1,
	GOTO = 2,
	ASSG = 3,
	IFEL = 4,
	ASSE = 5,
	ASSU = 6,
	NTHR = 7,
	ETHR = 8,
	ATOM = 9,
	EATM = 10,
	BCST = 11,
	WAIT = 12,
	SIGN = 13,
	CALL = 14
};

/**
 * @brief overloading operator <<
 * @param out
 * @param s
 * @return output stream:
 *         print an edge in cfg
 */
inline ostream& operator <<(ostream& out, const type_stmt& t) {
	switch (t) {
	case type_stmt::SKIP:
		out << (1);
		break;
	case type_stmt::GOTO:
		out << (2);
		break;
	case type_stmt::ASSG:
		out << (3);
		break;
	case type_stmt::IFEL:
		out << (4);
		break;
	case type_stmt::ASSE:
		out << (5);
		break;
	case type_stmt::ASSU:
		out << (6);
		break;
	case type_stmt::NTHR:
		out << (7);
		break;
	case type_stmt::ETHR:
		out << (8);
		break;
	case type_stmt::ATOM:
		out << (9);
		break;
	case type_stmt::EATM:
		out << (10);
		break;
	case type_stmt::BCST:
		out << (11);
		break;
	case type_stmt::WAIT:
		out << (12);
		break;
	case type_stmt::SIGN:
		out << (13);
		break;
	case type_stmt::CALL:
		out << (14);
		break;
	default:
		throw runtime_error("Illegal statement!");
		break;
	}
	return out;
}

/**
 * @brief data structure <expr>: build an expression from a list of strings
 */
class expr {
public:
	expr();
	expr(const deque<symbol>& sexpr);

	~expr();

	/// getter
	const deque<symbol>& get_sexpr() const {
		return sexpr;
	}

	const bool is_void() const {
		return sexpr.empty();
	}

	const bool is_const() const {
		for (const auto& s : sexpr)
			if (s > solver::CONST_N)
				return false;
		return true;
	}

	const deque<deque<symbol> >& get_splited() const {
		return splited;
	}

	const sool eval(const state_v& sv, const state_v& lv) const;
	sool eval(const state_v& sh, const state_v& lo);
	const sool eval(const state_v& sv, const state_v& lv, const state_v& _sv,
			const state_v& _lv) const;
	sool eval(const state_v& sh, const state_v& lo, const state_v& _sv,
			const state_v& _lv);
private:
	deque<symbol> sexpr;
	deque<deque<symbol>> splited;

	friend ostream& operator <<(ostream& out, const expr& e);
};

/**
 * @brief data structure <stmt>: store statement information, it wraps
 *        the type and condition (if applicable) of a statement; it is
 *        labeled to a edge in a control flow graph.
 */
class statement {
public:
	statement();
	statement(const type_stmt& type);
	statement(const type_stmt& type, const expr& condition);
	statement(const statement& s);
	~statement();

	/**
	 * @brief return the precondition if computing the preimage, or
	 *        postcondition if computing the postimage.
	 * @return precondition or postcondition
	 */
	const expr& get_condition() const {
		return condition;
	}

	/**
	 * @brief return statement type
	 * @return
	 */
	type_stmt get_type() const {
		return type;
	}

private:
	type_stmt type; /// statement type
	expr condition; /// precondition or postcondition

	friend ostream& operator <<(ostream& out, const statement& s);
};

/**
 * @brief data structure: edge, the base class
 *        src : source pc of CFG edge
 *        dest: destination pc of CFG edge
 *        stmt: statement type attached to edge (src, dest)
 */
class edge {
public:
	edge();
	edge(const size_pc& src, const size_pc& dest, const statement& st);
	edge(const size_pc& src, const size_pc& dest, const type_stmt& type);
	edge(const size_pc& src, const size_pc& dest, const type_stmt& type,
			const expr& condition);
	edge(const edge& e);
	~edge();

	size_pc get_dest() const {
		return dest;
	}

	size_pc get_src() const {
		return src;
	}

	const statement& get_stmt() const {
		return stmt;
	}

private:
	size_pc src;
	size_pc dest;
	statement stmt;

	friend ostream& operator <<(ostream& out, const edge& e);
};

using adj_list = vector<deque<edge>>;
using expr_vec = vector<expr>;

///////////////////////////////////////////////////////////////////////////////
/// from here: this data structure defines the expressions in a parallel
///            assignment statement
///////////////////////////////////////////////////////////////////////////////
struct assignment {
	expr_vec sh;
	expr_vec lo;
	assignment() :
			sh(refs::SV_NUM), lo(refs::LV_NUM) {

	}
	assignment(const ushort& s_num, const ushort& l_num) :
			sh(s_num), lo(l_num) {

	}
	~assignment() {
	}

	friend ostream& operator <<(ostream& out, const assignment& s);
};

///////////////////////////////////////////////////////////////////////////////
/// from here: this data structure defines the Control Flow Graph used in
///            pre-/post-images. This is the most important data structure.
///            A CFG is built in parsing a Boolean program via a Parser.
///////////////////////////////////////////////////////////////////////////////
class cfg {
public:
	cfg();
	cfg(const size_pc& size_A);
	cfg(const adj_list& A, const unordered_map<size_pc, assignment>& as);
	~cfg();

	const adj_list& get_A() const {
		return A;
	}

	const unordered_map<size_pc, assignment>& get_assignments() const {
		return assignments;
	}

	void add_edge(const size_pc& src, const size_pc& dest,
			const type_stmt& type);
	void add_edge(const size_pc& src, const size_pc& dest,
			const type_stmt& type, const expr& condition);
	void add_assignment(const size_pc& pc, const assignment& a);

private:
	adj_list A;
	unordered_map<size_pc, assignment> assignments;

	friend ostream& operator <<(ostream& out, const cfg& g);
};

} /* namespace bopp */

#endif /* API_CFG_HH_ */
