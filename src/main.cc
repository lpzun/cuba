//============================================================================
// Name        : Context-UnBounded Analysis Concurrent Software
// Author      : Peizun Liu
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "utils/cmd.hh"
#include "cuba/cuba.hh"

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

		/// Problem Instances
		const string& filename = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--input-file");
		const string& initl = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--initial");
		const string& final = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--target");
		const string& mode = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--mode");

		prop::OPT_PRINT_ADJ = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::PROB), "--list-input");
		prop::OPT_PROB_REACHABILITY = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::PROB), "--reachability");

		/// Sequential Mode
		prop::OPT_SEQ_ATM = cmd.arg_bool(cmd_line::get_opt_index(opt_type::SEQ),
				"--automaton");

		/// Concurrent Mode
		const string& k = cmd.arg_value(cmd_line::get_opt_index(opt_type::CON),
				"--ctx-bound");
		const string& n = cmd.arg_value(cmd_line::get_opt_index(opt_type::CON),
				"--threads");
		const bool is_explicit = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::CON), "--explicit");
		prop::OPT_PARAMETERIZED = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::CON), "--parameterized");

		/// Other Options
		prop::OPT_PRINT_CMD = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::OTHER), "--cmd-line");
		prop::OPT_PRINT_ALL = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::OTHER), "--all");

		if (mode == "O") {
			cout << "Overapproximation mode\n";
			cout << filename << " " << initl << "\n";
			processor proc(initl, filename);
		} else if (mode == "S") {
			cout << "sequential mode\n";
			cout << filename << " " << initl << " " << final << "\n";
			cout << "sequential computation is not set up yet! \n";
		} else {
			cout << "concurrent mode......\n";
			if (prop::OPT_PARAMETERIZED && std::stoul(n) == 0) {
				throw cuba_runtime_error(
						"Please specify the number of threads!");
			}
			if (is_explicit) {
				cout << "explicit exploration......\n";
				explicit_cuba ecuba(initl, final, filename, std::stoul(n));
				ecuba.context_bounded_analysis(std::stoul(k));
			} else {
				cout << "symbolic exploration......\n";
				symbolic_cuba scuba(initl, final, filename, std::stoul(n));
				scuba.context_bounded_analysis(std::stoul(k));
			}
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
