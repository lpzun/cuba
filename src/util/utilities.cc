/**
 * @brief utilities.cc
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#include "utilities.hh"

namespace cuba {

/**
 * @brief split a string into a vector of strings via a delimiter
 * @param s: object string
 * @param delim: a delimiter
 * @return a vector of string
 */
deque<string> pds_parser::split(const string &s, const char& delim) {
    deque<string> elems;
    std::stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        elems.emplace_back(item);
    }
    return elems;
}

/**
 *
 * @param s_ts
 * @param delim
 * @return bool
 */
thread_state pds_parser::create_thread_state_from_str(const string& s_ts,
        const char& delim) {
    deque<string> vs_ts = pds_parser::split(s_ts, delim);
    if (vs_ts.size() != 2) {
        throw("The format of thread state is wrong.");
    }
    return thread_state(stoi(vs_ts[0]), stoi(vs_ts[1]));
}

/**
 * @brief remove the comments of .ttd files
 * @param in
 * @param filename
 * @param comment
 */
void pds_parser::remove_comments(istream& in, const string& filename,
        const string& comment) {
    ofstream out(filename.c_str());
    remove_comments(in, out, comment);
}

/**
 * parse the type of stack operation
 * +: push op
 * -: pop op
 * !: overwrite op
 * @param c
 * @return type_stack_operation
 */
type_stack_operation pds_parser::parse_type_stack_operation(const char& c) {
    switch (c) {
    case '+':
        return type_stack_operation::PUSH;
    case '-':
        return type_stack_operation::POP;
    case '!':
        return type_stack_operation::UPDATE;
    default:
        throw cuba_runtime_error("illegal transition!");
    }
}

/**
 * parse the type of synchronization
 * +: fork a new thread
 * ~: broadcast
 * -: normal transition, no synchronization
 * @param c
 * @return type_synchronization
 */
type_synchronization pds_parser::parse_type_synchronization(const char& c) {
    switch (c) {
    case '+':
        return type_synchronization::FORK;
    case '~':
        return type_synchronization::BRCT;
    case '-':
        return type_synchronization::NORM;
    default:
        throw cuba_runtime_error("illegal transition!");
    }

}

/**
 * @brief remove the comments of .ttd files
 * @param in
 * @param out
 * @param comment
 */
void pds_parser::remove_comments(const string& in, string& out,
        const string& comment) {
    std::istringstream instream(in);
    std::ostringstream outstream;
    remove_comments(instream, outstream, comment);
    out = outstream.str();
}

/**
 * remove comments
 * @param in
 * @param out
 * @param comment
 */
void pds_parser::remove_comments(istream& in, ostream& out,
        const string& comment) {
    string line;
    while (getline(in, line = "")) {
        const size_t comment_start = line.find(comment);
        out
                << (comment_start == string::npos ?
                        line : line.substr(0, comment_start)) << endl;
    }
}

/**
 *
 * @param in
 * @param line
 * @param eol
 * @return bool
 */
bool pds_parser::getline(istream& in, string& line, const char& eol) {
    char c = 0;
    while (in.get(c) ? c != eol : false)
        line += c;
    return c != 0;
}

} /* namespace cuba */
