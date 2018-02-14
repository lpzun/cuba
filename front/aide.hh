/*************************************************************************************
 ** Name:       BoPP: parser
 ** Authors:    Peizun Liu
 ** Version:    0.5
 ** Copyright:  It belongs to Thomas Wahl's group in CAR Lab, CCIS, NEU
 ** Create on:  Feb, 2014
 ** Modified :  Jan, 2016
 ** Decription: BoPP is a Boolean Program Parser written with C++. It aims at parsing
 *              Boolean programs to generate a control folow graph and the correspon-
 *              ding weakest preconditions (strongest postconditions) for each state-
 *              ment when computing a preimage (postimage).
 *
 *              parser:
 *              v0.5: adding the forward-based CFG and so on
 ************************************************************************************/

#ifndef BPP_AIDE_HH_
#define BPP_AIDE_HH_

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <set>
#include <deque>
#include <vector>
#include <stack>
#include <memory>

#include "../api/cfg.hh"
#include "../util/excp.hh"

using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::cerr;

using std::set;
using std::map;
using std::deque;
using std::vector;
using std::stack;
using std::pair;

namespace ijit {

/**
 * @brief the mode of parser: probably compute prev-/post-images of a global
 *         state.
 */
enum class mode {
    PREV, POST
};

/**
 * @brief this is auxiliary class for Boolean program parser
 *        paide: parser aide
 */
class paide {
public:
    paide(const mode& m);
    ~paide();

    size_pc lineno; /// initialize the lineno
    size_pc ipc; /// the source of cfg edge
    ushort s_vars_num; /// the number of shared variables
    ushort l_vars_num; /// the number of local variables
    map<string, ushort> s_vars_list; /// store shared variables and indices
    map<string, ushort> l_vars_list; /// store local  variables and indices
    map<ushort, sool> s_vars_init; /// record values of initial shared states
    map<ushort, sool> l_vars_init; /// record values of initial local  states

    set<size_pc> suc_pc_set; /// store the succeeding pcs
    deque<symbol> expr_in_list; /// the expression symbol list

    /// the following two containers are used to store the expression included
    /// in the parallel assignment statements
    deque<symbol> assg_stmt_lhs; /// the RHS of parallel assignment
    deque<deque<symbol>> assg_stmt_rhs; /// the LHS of parallel assignment

    set<size_pc> asse_pc_set; /// record all of PCs associated with assertions

    cfg cfg_G; /// store the control flow graph

    ////////////////// function list //////////////////

    /// initial states
    void add_vars(const string& var, const sool& val, const bool& is_shared);
    void init_vars(const string& var, const sool& val);
    /// control flow graph function list
    bool is_pc_unique(const size_pc& pc);

    void add_edge(const size_pc& src, const type_stmt& type);
    void add_edge(const size_pc& src, const size_pc& dest,
            const type_stmt& type, const bool& is_condition = false);

    /// expression
    inline void add_to_expr_in_list(const symbol& s);
    symbol encode(const string& var);
    pair<symbol, bool> decode(const symbol& idx);

    ////////////////// unit test //////////////////
    void print_control_flow_graph();
    void print_parallel_assg_stmt();
    void print_vars_list();

private:
    set<size_pc> all_pc_set; /// to determine if pc is unique or not
    mode m;
    assignment create_assignment();
};

/**
 * @brief to add the expression symbols to a list
 * @param symbol
 */
inline void paide::add_to_expr_in_list(const symbol& s) {
    expr_in_list.emplace_back(s);
}

} /* namespace otf */
#endif /* BPP_AIDE_HH_ */
