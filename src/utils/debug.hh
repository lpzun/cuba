/**
 * For debugging
 *
 * @name debug.hh
 *
 * @date  : Oct 2, 2013
 * @author: TODO
 */

#ifndef DEBUG_HH_
#define DEBUG_HH_

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
};
}
#endif /* DEBUG_HH_ */
