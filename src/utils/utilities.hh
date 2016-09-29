/**
 * @brief utilities.hh
 *
 * @date  : Aug 29, 2016
 * @author: lpzun
 */

#ifndef UTILS_UTILITIES_HH_
#define UTILS_UTILITIES_HH_

#include <map>

#include "../utils/heads.hh"

namespace cuba {

enum class alignment {
	LEFTJUST, RIGHTJUST, CENTERED
};

class algs {
public:
	template<typename T>
	static int compare(const vector<T>& v1, const vector<T>& v2,
			const bool& is_symmetry);

	template<class T>
	static string widthify(const T& x, const ushort& width = 0,
			const alignment& c = alignment::CENTERED, const char& fill = ' ');

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

template<class T>
string algs::widthify(const T& x, const ushort& width, const alignment& c,
		const char& fill) {
	std::ostringstream os;
	os << x;
	string s = os.str();

	ushort n = s.size();
	if (n >= width)
		return s;
	ushort addlength = width - n;
	string result;
	switch (c) {
	case alignment::LEFTJUST:
		result = s + string(addlength, fill);
		break;
	case alignment::RIGHTJUST:
		result = string(addlength, fill) + s;
		break;
	case alignment::CENTERED:
		result = (
				addlength % 2 == 0 ?
						string(addlength / 2, fill) + s
								+ string(addlength / 2, fill) :
						string((addlength - 1) / 2, fill) + s
								+ string((addlength + 1) / 2, fill));
		break;
	}
	return result;
}
} /* namespace cuba */

#endif /* UTILS_UTILITIES_HH_ */
