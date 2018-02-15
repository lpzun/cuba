/**
 * prop.hh This is a property class
 *
 * @date   Aug 29, 2016
 * @author TODO
 */

#ifndef UTILS_PROP_HH_
#define UTILS_PROP_HH_

#include "heads.hh"

namespace ruba {

class prop {
public:
	prop();
	~prop();

	static bool OPT_PRINT_ALL;
	static bool OPT_PRINT_CMD;
	static bool OPT_PRINT_ADJ;

	static bool OPT_PROB_REACHABILITY;
	static bool OPT_SEQ_ATM;

	static const string COMMENT;

	static const char SHARED_LOCAL_DELIMITER;
	static const char STACK_DELIMITER;
	static const char THREAD_DELIMITER;
};

} /* namespace ruba */

#endif /* UTILS_PROP_HH_ */
