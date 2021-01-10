/**
 * prop.hh This is a property class
 *
 * @date   Aug 29, 2016
 * @author TODO
 */

#ifndef UTILS_FLAGS_HH_
#define UTILS_FLAGS_HH_

#include "heads.hh"

namespace ruba {

class flags {
public:
	flags();
	~flags();

	static bool OPT_PRINT_ALL;
	static bool OPT_PRINT_CMD;
	static bool OPT_PRINT_ADJ;
	static bool OPT_FILE_DUMP;

	static bool OPT_PROB_REACHABILITY;
	static bool OPT_GLOBAL_STATE;
	static bool OPT_SEQ_ATM;
	static bool OPT_NESTED_MATCH;
	static string OPT_MATCHING_FILE;
	static size_t OPT_Z_APPROXIMATION_BOUND;

	static const string COMMENT;

	static const char SHARED_LOCAL_DELIMITER;
	static const char STACK_DELIMITER;
	static const char THREAD_DELIMITER;
};

} /* namespace ruba */

#endif /* UTILS_FLAGS_HH_ */
