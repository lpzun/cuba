/*
 * log.cc
 *
 * @date  : Mar 4, 2018
 * @author: lpzun
 */

#include <log.hh>

namespace ruba {
string logger::MSG_ERROR = "ERROR: ";
string logger::MSG_WARNING = "WARNING: ";
string logger::MSG_SEPARATOR = "======================================\n";
string logger::MSG_TR_PLATEAU_AT_K = "=> sequence T(R) plateaus at ";
string logger::MSG_TR_COLLAPSE_AT_K = "=> sequence T(R) collapses at ";
string logger::MSG_NUM_VISIBLE_STATES = "the number of new visible states: ";

} /* namespace ruba */
