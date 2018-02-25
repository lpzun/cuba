/******************************************************************************
 * Synopsis	 [CUBA: Context-UnBounded Analysis for Concurrent Programs]
 *
 * Developer [<Anonymous> Developer]
 *
 * (C) 2017 - 2018 <Anonymous> Developer
 *
 * All rights reserved. Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *
 *    This product includes software developed by <Anonymous> @ <Anonymous>'s
 *    group, Northeastern University, United States and its contributors.
 *
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS `AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <iostream>

#include "cmd.hh"
#include "cuba.hh"

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
		if (filename == "X")
			throw cuba_runtime_error("Please assign an input CPDS!");

		const string& initl = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--initial");
		if (initl == "X")
			throw cuba_runtime_error("Please assign an initial state!");

		const string& final = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::PROB), "--target");

		/// Concurrent Mode
		const string& k_bound = cmd.arg_value(
				cmd_line::get_opt_index(opt_type::CON), "--res-bound");
		size_k k = k_bound.size() == 0 ? 0 : std::stoul(k_bound);

		const bool is_explicit = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::CON), "--explicit");

		/// Other Options
		prop::OPT_PRINT_ALL = cmd.arg_bool(
				cmd_line::get_opt_index(opt_type::OTHER), "--all");

		if (is_explicit) {
			explicit_cuba ecuba(initl, final, filename);
			ecuba.context_unbounded_analysis(k);
		} else {
			symbolic_cuba scuba(initl, final, filename);
			scuba.context_unbounded_analysis(k);
		}
		cout << "======================================" << endl;

	} catch (const cmd::cmd_runtime_error& e) {
		cerr << "ERROR: " << e.what() << endl;
	} catch (const ruba::cuba_runtime_error& e) {
		cerr << "ERROR: " << e.what() << endl;
	} catch (const ruba::cuba_exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	} catch (const std::exception& e) {
		cerr << "ERROR: " << e.what() << endl;
	}
	return 0;
}
