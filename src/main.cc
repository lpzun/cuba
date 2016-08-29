//============================================================================
// Name        : Context-UnBounded Analysis Concurrent Software
// Author      : Peizun Liu
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "util/cmd.hh"
#include "cuba/cuba.hh"

using namespace std;
using namespace cmd;
using namespace cuba;

int main(const int argc, const char * const * const argv) {
    try {
        cmd_line cmd;
        try {
            cmd.get_command_line(argc, argv);
        } catch (cmd_line::help) {
            return 0;
        }
    } catch (const cmd::cmd_runtime_error& e) {
        e.what();
    } catch (const cuba::cuba_runtime_error& e) {
        e.what();
    } catch (const cuba::cuba_exception& e) {
        e.what();
    } catch (const std::exception& e) {
        e.what();
    }
    return 0;
}
