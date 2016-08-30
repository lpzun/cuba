/**
 * @brief utilities.hh
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#ifndef UTIL_UTILITIES_HH_
#define UTIL_UTILITIES_HH_

#include "heads.hh"

namespace cuba {
class algs {
public:
    template<typename T>
    static int compare(const vector<T>& v1, const vector<T>& v2,
            const bool& is_symmetry);

private:

};
} /* namespace cuba */

#endif /* UTIL_UTILITIES_HH_ */
