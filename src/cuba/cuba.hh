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
    CUBA(const string& filename, const string& init, const string& final);
    ~CUBA();

private:
    map<thread_state, vertex> state_to_ID;
    vector<thread_state> active_Q; /// active control states
    vector<transition<vertex>> active_R;/// active transitions
    adj_list PDS;

    void context_unbounded_analysis(const size_t& n);

    void parse_PDS(const string& filename);
    thread_state parse_TS(const string& s);
};

} /* namespace cuba */

#endif /* CUBA_CUBA_HH_ */
