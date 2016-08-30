/**
 * @brief cuba.hh
 *
 * @date  : Aug 28, 2016
 * @author: lpzun
 */

#ifndef CUBA_CUBA_HH_
#define CUBA_CUBA_HH_

#include "../util/utilities.hh"
#include "../util/prop.hh"

namespace cuba {

using vertex = id_thread_state;
using edge = id_transition;
using adj_list = map<vertex, deque<edge>>;

/////////////////////////////////////////////////////////////////////////
/// PART 1. The following are the declarations for context-unbounded
/// analysis.
/////////////////////////////////////////////////////////////////////////
class CUBA {
public:
    CUBA(const string& filename, const string& initl, const string& final);
    ~CUBA();

    void context_unbounded_analysis(const size_t& n);

private:
    /// Part 1: parse a pushdown system (PDS)
    map<thread_state, vertex> mapping_Q; /// mapping a control state to its ID
    vector<thread_state> active_Q;       /// active control states
    vector<transition<vertex>> active_R; /// active transitions
    adj_list PDS;

    void parse_PDS(const string& filename);
    thread_state parse_TS(const string& s);

    thread_state initl_TS;
    thread_state final_TS;

    uint reachable_thread_states(const size_t& n);
};

/////////////////////////////////////////////////////////////////////////
/// PART 2. The following are the utilities for PDS file parser.
///
/////////////////////////////////////////////////////////////////////////
class pds_parser {
public:
    static void remove_comments(istream& in, const string& filename,
            const string& comment);
    static void remove_comments(const string& in, string& out,
            const string& comment);

    static thread_state create_thread_state_from_str(const string& s_ts,
            const char& delim = '|');
    static global_state create_global_state_from_str(const string& s_ts,
            const char& delim = '|');

    static type_stack_operation parse_type_stack_operation(const char& c);
    static type_synchronization parse_type_synchronization(const char& c);

private:
    static void remove_comments(istream& in, ostream& out,
            const string& comment);
    static bool getline(istream& in, string& line, const char& eol = '\n');

    static deque<string> split(const string &s, const char& delim);

};

} /* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
