//============================================================================
// Name        : Context-UnBounded Analysis Concurrent Software
// Author      : Peizun Liu
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "cuba.hh"
#include "cmd.hh"

using namespace std;
using namespace cmd;
using namespace cuba;

/**
 *
 * @param argc
 * @param argv
 * @return int
 *
 */
int main(const int argc, const char * const * const argv) {
	try {
		cmd_line cmd;
		try {
			cmd.get_command_line(argc, argv);
		} catch (cmd_line::help) {
			return 0;
		}

		prop::OPT_PRINT_ADJ = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::PROB), "--pushdown");
		prop::OPT_PRINT_CMD = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::OTHER), "--cmd-line");
		prop::OPT_PRINT_ALL = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::OTHER), "--all");

		const string& filename = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--input-file");
		const string& initl = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--initial");
		const string& final = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--target");

		const string& n = cmd.arg_value(cmd_line::get_opt_index(opt_type::CON),
				"--threads");
		const string& k = cmd.arg_value(cmd_line::get_opt_index(opt_type::CON),
				"--ctx-bound");

		prop::OPT_REACHABILITY = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::SEQ), "--reachability");

		CUBA cuba(initl, final, filename);
		cuba.context_bounded_analysis(std::stoul(k), std::stoul(n));

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
