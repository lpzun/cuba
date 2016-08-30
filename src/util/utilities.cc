/**
 * @brief utilities.cc
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#include "utilities.hh"

namespace cuba {

/**
 * compare two vectors. Remark that we assume the sizes of
 * these two vectors are the same.
 *
 * @param v1
 * @param v2
 * @param is_symmetry
 * @return int
 *         -1: v1 < v2
 *          0: v1 = v2
 *         +1: v1 > v2;
 */
template<typename T>
int algs::compare(const vector<T>& v1, const vector<T>& v2,
        const bool& is_symmetry) {
    if (v1.size() != v2.size()) {
        throw cuba_runtime_error("algs::compare: sizes are not equal!");
    }

    if (is_symmetry) {
        std::sort(v1.begin(), v1.end());
        std::sort(v2.begin(), v2.end());
    }

    auto iv1 = v1.begin();
    auto iv2 = v2.begin();
    while (iv1 != v1.end()) {
        if (*iv1 < *iv2) {
            return -1;
        }
        if (*iv1 > iv2) {
            return 1;
        }
        ++iv1, ++iv2; /// *iv1 == *iv2
    }
    return 0;
}

} /* namespace cuba */
