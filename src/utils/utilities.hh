/**
 * @brief utilities.hh
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#ifndef UTILS_UTILITIES_HH_
#define UTILS_UTILITIES_HH_

#include "../utils/heads.hh"

namespace cuba {
class algs {
public:
	template<typename T>
	static int compare(const vector<T>& v1, const vector<T>& v2,
			const bool& is_symmetry);

private:

};

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

	auto tv1(v1);
	auto tv2(v2);
	if (is_symmetry) {
		std::sort(tv1.begin(), tv1.end());
		std::sort(tv2.begin(), tv2.end());
	}

	auto iv1 = tv1.cbegin();
	auto iv2 = tv2.cbegin();
	while (iv1 != tv1.cend()) {
		if (*iv1 < *iv2)
			return -1;
		if (*iv1 > *iv2)
			return 1;
		++iv1, ++iv2; /// *iv1 == *iv2
	}
	return 0;
}
} /* namespace cuba */

#endif /* UTILS_UTILITIES_HH_ */
