/**
 * @brief prop.hh
 * This is a property class
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#ifndef UTILS_PROP_HH_
#define UTILS_PROP_HH_

#include "heads.hh"

namespace cuba {

class prop {
public:
	prop();
	~prop();

	static bool OPT_PRINT_ALL;
	static bool OPT_PRINT_CMD;
	static bool OPT_PRINT_ADJ;

	static bool OPT_REACHABILITY;

	static const string COMMENT;
};

} /* namespace cuba */

#endif /* UTILS_PROP_HH_ */
