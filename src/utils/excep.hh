/**
 * @name excep.hh
 *
 * @date Jan 12, 2015
 * @author Peizun Liu
 */

#ifndef EXCEP_HH_
#define EXCEP_HH_

#include <stdexcept>
#include <exception>
#include <string>

using std::runtime_error;
using std::exception;
using std::string;

namespace ruba {

/**
 * @brief customized runtime error class
 */
class cuba_runtime_error: public runtime_error {
public:
    cuba_runtime_error() :
            runtime_error("") {
    }
    cuba_runtime_error(const std::string& msg) :
            runtime_error(msg) {
    }
};

/**
 * @brief customized exception
 */
class cuba_exception: public exception {
public:
    string message;
    short code;

    inline cuba_exception() :
            message(""), code(-1) {

    }

    inline cuba_exception(const string& message, const short& code = -1) :
            message(message), code(code) {
    }    // must provide some message

    virtual const char* what() const throw () {
        return message.c_str();
    }
};

}
#endif /* EXCEP_HH_ */
