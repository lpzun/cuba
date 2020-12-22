/**
 * prop.cc
 *
 * @date  : Aug 29, 2016
 * @author: TODO
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
bool prop::OPT_PRINT_TOP_R = false;

bool prop::OPT_PROB_REACHABILITY = false;

bool prop::OPT_SEQ_ATM = false;

const string prop::COMMENT = "#";

const char prop::SHARED_LOCAL_DELIMITER = '|';
const char prop::STACK_DELIMITER = '.';
const char prop::THREAD_DELIMITER = ',';

string prop::MSG_SEPARATOR = "======================================\n";
string prop::MSG_TR_PLATEAU_AT_K = "=> sequence T(R) plateaus at ";
string prop::MSG_TR_COLLAPSE_AT_K = "=> sequence T(R) collapses at ";
string prop::MSG_NUM_VISIBLE_STATES = "the number of new visible states: ";

}
/* namespace ruba */
