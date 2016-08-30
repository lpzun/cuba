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

        prop::OPT_PRINT_ADJ = cmd.arg_bool(cmd_line::prob_inst_opts(),
                "--adj-list");

        prop::OPT_PRINT_CMD = cmd.arg_bool(cmd_line::other_opts(),
                "--cmd-line");
        prop::OPT_PRINT_ALL = cmd.arg_bool(cmd_line::other_opts(), "--all");

        const string& filename = cmd.arg_value(cmd_line::prob_inst_opts(),
                "--input-file");
        const string& initl = cmd.arg_value(cmd_line::prob_inst_opts(),
                "--initial");
        const string& final = cmd.arg_value(cmd_line::prob_inst_opts(),
                "--target");

        const string& n = cmd.arg_value(cmd_line::exp_mode_opts(), "--threads");

        cout << filename << " " << initl << " " << final << " " << n
                << endl;

        CUBA cuba(filename, initl, final);

        cuba.context_unbounded_analysis(std::stoul(n));
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
