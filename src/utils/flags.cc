/**
 * prop.cc
 *
 * @date  : Aug 29, 2016
 * @author: TODO
 */

#include <flags.hh>

namespace ruba {

flags::flags() {
}

flags::~flags() {
}

bool flags::OPT_PRINT_ADJ = false;
bool flags::OPT_PRINT_CMD = false;
bool flags::OPT_PRINT_ALL = false;
bool flags::OPT_FILE_DUMP = false;

bool flags::OPT_PROB_REACHABILITY = false;
bool flags::OPT_GLOBAL_STATE = false;

bool flags::OPT_SEQ_ATM = false;
bool flags::OPT_NESTED_MATCH = false;
string flags::OPT_MATCHING_FILE = "X";
size_t flags::OPT_Z_APPROXIMATION_BOUND = 1;

const string flags::COMMENT = "#";

const char flags::SHARED_LOCAL_DELIMITER = '|';
const char flags::STACK_DELIMITER = '.';
const char flags::THREAD_DELIMITER = ',';

}
/* namespace ruba */
