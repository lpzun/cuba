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

    vector<vector<bool>> reached_Q;
};

} /* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
