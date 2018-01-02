/**
 * @brief prop.cc
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#include "prop.hh"

namespace ruba {

prop::prop() {
}

prop::~prop() {
}

bool prop::OPT_PRINT_ADJ = false;
bool prop::OPT_PRINT_CMD = false;
bool prop::OPT_PRINT_ALL = false;

bool prop::OPT_PROB_REACHABILITY = false;
bool prop::OPT_PARAMETERIZED = false;

bool prop::OPT_SEQ_ATM = false;

const string prop::COMMENT = "#";

const char prop::SHARED_LOCAL_DELIMITER = '|';
const char prop::STACK_DELIMITER = '.';
const char prop::THREAD_DELIMITER = ',';

} /* namespace ruba */
