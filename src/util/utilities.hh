/**
 * @brief utilities.hh
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#ifndef UTIL_UTILITIES_HH_
#define UTIL_UTILITIES_HH_

#include "heads.hh"
#include "state.hh"

namespace cuba {

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

#endif /* UTIL_UTILITIES_HH_ */
