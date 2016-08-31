/**
 * @brief prop.hh
 * This is a property class
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#ifndef UTIL_PROP_HH_
#define UTIL_PROP_HH_

#include "state.hh"

namespace cuba {

class prop {
public:
    prop();
    ~prop();

    static bool OPT_PRINT_ALL;
    static bool OPT_PRINT_CMD;
    static bool OPT_PRINT_ADJ;

    static const string COMMENT;
};

} /* namespace cuba */

#endif /* UTIL_PROP_HH_ */
