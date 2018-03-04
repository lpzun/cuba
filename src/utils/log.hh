/*
 * log.hh
 *
 * @date  : Mar 4, 2018
 * @author: lpzun
 */

#ifndef UTILS_LOG_HH_
#define UTILS_LOG_HH_

#include <string>

using std::string;

#define NDEBUG

#ifndef NDEBUG
#define DEBUG_MODE
#else
#endif

#ifdef DEBUG_MODE
#  define DBG_LOG(x) std::cerr << x <<"\n";
#  define DBG_STD(x) x;
#  define DBG_LOC() std::cerr <<__func__<<" I am here ..."<<"\n";
#else
#  define DBG_LOG(x)
#  define DBG_STD(x)
#  define DBG_LOC();
#endif // DEBUG

#define STATISTIC

namespace ruba {

class debugger {
public:
	debugger();
	~debugger();
};

class logger {
public:
	logger() {
	}
	~logger() {

	}

	static string MSG_ERROR;
	static string MSG_WARNING;

	static string MSG_EXP_EXPLORATION;
	static string MSG_SYM_EXPLORATION;

	static string MSG_SEPARATOR;
	static string MSG_TR_PLATEAU_AT_K;
	static string MSG_TR_COLLAPSE_AT_K;
	static string MSG_NUM_VISIBLE_STATES;
};

} /* namespace ruba */

#endif /* UTILS_LOG_HH_ */
