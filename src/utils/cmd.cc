/**
 *  cmd.cc
 *
 *  @date  : Nov 18, 2014
 *  @author: TODO
 */

#include "cmd.hh"

namespace cmd {

/**
 *
 * @param x
 * @param width
 * @param c
 * @param fill
 * @return
 */
template<class T>
string widthify(const T& x, const ushort& width, const alignment& c,
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

/**
 *
 * @param opt
 * @return
 */
string cmd_line::get_opt_types(const opt_type& opt) {
	switch (opt) {
	case opt_type::PROB:
		return "Problem Instance:";
	case opt_type::SEQ:
		return "Sequential Mode:";
	case opt_type::CON:
		return "Concurrent Mode:";
	case opt_type::OTHER:
		return "Other Options:";
	default:
		return "";
	}
}

/**
 *
 * @param opt
 */
short cmd_line::get_opt_index(const opt_type& opt) {
	switch (opt) {
	case opt_type::PROB:
		return 1;
	case opt_type::SEQ:
		return 2;
	case opt_type::CON:
		return 3;
	case opt_type::OTHER:
		return 4;
	default:
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
cmd_line::cmd_line() :
		SHORT_HELP_OPT("-h"), LONG_HELP_OPT("--help"), SHORT_VERSION_OPT("-v"), ///
		LONG_VERSION_OPT("--version"), VERSION("v1.0"), name_width(0), ///
		xwidth(10), help_message(
				"Use " + string(SHORT_HELP_OPT) + " or " + string(LONG_HELP_OPT)
						+ " for help"), v_info(), cmd_options(), cmd_switches(), types() {
	this->create_argument_list();
}

cmd_line::~cmd_line() {
}

/**
 *
 * @param argc
 * @param argv
 */
void cmd_line::get_command_line(const int argc,
		const char* const * const argv) {
	vector<string> args(argc - 1);
	for (int i = 0; i < argc - 1; i++) {
		args[i] = argv[i + 1];
	}
	this->get_command_line(argv[0], args);
}

/**
 * @brief Parsing command line
 * @param prog
 * @param args
 */
void cmd_line::get_command_line(const string& prog,
		const vector<string>& args) {
	for (auto iarg = args.begin(), iend = args.end(); iarg != iend; ++iarg) {
		const string& arg = *iarg;
		if (arg == SHORT_HELP_OPT || arg == LONG_HELP_OPT) {
			this->print_usage_info(prog);
			throw help();
		} else if (arg == SHORT_VERSION_OPT || arg == LONG_VERSION_OPT) {
			cout << v_info << endl;
			throw help();
		} else {
			bool flag = true;
			for (ushort i = 0; i < types.size() && flag; ++i) {
				auto iopts = cmd_options.find(i);
				if (iopts != cmd_options.end())
					for (auto iopt = iopts->second.begin();
							iopt != iopts->second.end() && flag; ++iopt) {
						if (arg == iopt->get_short_name()
								|| arg == iopt->get_long_name()) {
							++iarg;   /// the next string is the value for arg
							iopt->set_value(*iarg);
							if (flag)
								flag = false;
						}
					}

				auto iswts = cmd_switches.find(i);
				if (iswts != cmd_switches.end())
					for (auto iswt = iswts->second.begin();
							iswt != iswts->second.end() && flag; ++iswt) {
						if (arg == iswt->get_short_name()
								|| arg == iswt->get_long_name()) {
							iswt->set_value(true);
							if (flag)
								flag = false;
						}
					}

			}
			if (flag)
				throw cmd_runtime_error(
						"cmd_line::get_command_line: " + arg
								+ ": no such keyword argument.\n"
								+ help_message);
		}
	}
}

/**
 *
 * @param type
 * @param short_name
 * @param long_name
 * @param meaning
 * @param default_value
 */
void cmd_line::add_option(const short& type, const string& short_name,
		const string& long_name, const string& meaning,
		const string& default_value) {
	this->cmd_options[type].push_back(
			Options(type, short_name, long_name, meaning, default_value));
	this->name_width =
			name_width < short_name.size() + long_name.size() + xwidth ?
					short_name.size() + long_name.size() + xwidth : name_width;
}

/**
 *
 * @param type
 * @param short_name
 * @param long_name
 * @param meaning
 */
void cmd_line::add_switch(const short& type, const string& short_name,
		const string& long_name, const string& meaning) {
	this->cmd_switches[type].push_back(
			Switch(type, short_name, long_name, meaning));
	this->name_width =
			name_width < short_name.size() + long_name.size() + xwidth ?
					short_name.size() + long_name.size() + xwidth : name_width;
}

/**
 * @brief return bool value
 * @param type
 * @param arg
 * @return bool
 */
bool cmd_line::arg_bool(const unsigned short& type, const string& arg) {
	auto ifind = std::find(cmd_switches[type].begin(), cmd_switches[type].end(),
			Switch(type, arg));
	if (ifind == cmd_switches[type].end())
		throw cmd_runtime_error(
				"cmd_line:: argument " + arg + " does not exist!");
	return ifind->is_value();
}

/**
 * @brief return the value of argument
 * @param type
 * @param arg
 * @return string: the value of argument
 */
string cmd_line::arg_value(const short& type, const string& arg) {
	auto ifind = std::find(cmd_options[type].begin(), cmd_options[type].end(),
			Options(type, arg));
	if (ifind == cmd_options[type].end())
		throw cmd_runtime_error(
				"cmd_line:: argument " + arg + " does not exist!");
	return ifind->get_value();
}

/**
 * @brief print help information
 * @param prog_name
 * @param indent
 * @param out
 */
void cmd_line::print_usage_info(const string& prog_name, const ushort& indent,
		ostream& out) const {
	out << "\n";
	out << v_info << "\n";

	out << widthify("Usage:", this->name_width, alignment::LEFTJUST)
			<< widthify("Purpose:", 0, alignment::LEFTJUST) << "\n";
	out << " "
			<< widthify(
					prog_name + " " + SHORT_HELP_OPT + " [" + LONG_HELP_OPT
							+ "]", this->name_width, alignment::LEFTJUST)
			<< widthify("show help message", 0, alignment::LEFTJUST);
//	out << " "
//			<< widthify(prog_name + " -f source.pds ", this->name_width,
//					alignment::LEFTJUST)
//			<< widthify("check given program", 0, alignment::LEFTJUST);

	for (size_t i = 0; i < types.size(); i++) {
		if (i == 2)
			continue;
		auto iopts = cmd_options.find(i);
		auto iswts = cmd_switches.find(i);
		out << types[i] << "\n";
		if (iopts != cmd_options.end()) {
			for (auto iopt = iopts->second.begin(); iopt != iopts->second.end();
					++iopt) {
				out << " "
						<< widthify(
								iopt->get_short_name() + " ["
										+ iopt->get_long_name() + "] arg",
								this->name_width, alignment::LEFTJUST)
						<< widthify(
								widthify(
										iopt->get_meaning()
												+ (iopt->get_value().size()
														> 0 ?
														(" (default = "
																+ iopt->get_value()
																+ ")") :
														""),
										this->name_width + 2), 0,
								alignment::LEFTJUST) << "\n";
			}
		}
		if (iswts != cmd_switches.end()) {
			for (auto iswt = iswts->second.begin(); iswt != iswts->second.end();
					++iswt) {
				out << " "
						<< widthify(
								iswt->get_short_name() + " ["
										+ iswt->get_long_name() + "]",
								this->name_width, alignment::LEFTJUST)
						<< widthify(iswt->get_meaning(), 0, alignment::LEFTJUST)
						<< "\n";
			}
		}
		out << endl;
	}
}

/**
 * @brief create argument list
 */
void cmd_line::create_argument_list() {
	vector<string> types { ///
	get_opt_types(opt_type::DEFAULT), ///
	get_opt_types(opt_type::PROB), ///
	get_opt_types(opt_type::SEQ),  ///
	get_opt_types(opt_type::CON),  ///
	get_opt_types(opt_type::OTHER) };

	v_info = create_version_info();

	this->set_types(types);
	this->add_switch(get_opt_index(opt_type::OTHER), SHORT_HELP_OPT,
			LONG_HELP_OPT, "help information");

	/// problem instance
	this->add_option(get_opt_index(opt_type::PROB), "-f", "--input-file",
			"an input pushdown system", "X");
	this->add_option(get_opt_index(opt_type::PROB), "-i", "--initial",
			"an initial state", "0|0,0");
	this->add_option(get_opt_index(opt_type::PROB), "-a", "--target",
			"a target visible state", "0|0,0");
//	this->add_switch(get_opt_index(opt_type::PROB), "-l", "--list-input",
//			"show the input pushdown system");
//	this->add_option(get_opt_index(opt_type::PROB), "-m", "--mode",
//			(string("input program mode (default = C):\n") //
//			+ string(28, ' ') + " \'S\': sequential mode\n" //
//					+ string(28, ' ') + " \'C\': concurrent mode\n" //
//					+ string(28, ' ') + " \'O\': overapproximation mode" //
//			).c_str(), "");
	this->add_switch(get_opt_index(opt_type::PROB), "-r", "--reachability",
			"check the reachability of the given target state");

//	/// sequential mode
//	this->add_switch(get_opt_index(opt_type::SEQ), "-atm", "--automaton",
//			"show the pushdown store automaton");

	/// concurrent mode
//	this->add_option(get_opt_index(opt_type::CON), "-s", "--resource",
//			(string("the type of resource-unbounded analysis (default = C):\n") /// row 1
//			.append(string(28, ' ')).append(
//					" \'C\': CBA or CUBA (context-(un)bounded analysis)") /// row 2
//			.append(string(28, ' ')).append(
//					" \'W\': WBA or WUBA (write-(un)bounded analysis)") /// row 3
//			).c_str(), ""); /// row 3
	this->add_option(get_opt_index(opt_type::CON), "-k", "--res-bound",
			(string("resource bound, ").append(
					"performing resource-unbounded analysis if specify nothing") /// row 1
//			.append(string(27, ' ')).append(
//					"Per parameter -s, it has different meanings:\n") /// row 2
//			.append(string(29, ' ')).append(
//					"if -s = \'C\', then k represents the context bound") ///
//			.append("\n") ///
//			.append(string(29, ' ')).append(
//					"if -s = \'W\', then k represents the write access bound") ///
			), ""); /// row 4
	this->add_switch(get_opt_index(opt_type::CON), "-x", "--explicit",
			"run the explicit exploration assuming finite resource reachability holds");

//	/// other options
//	this->add_switch(get_opt_index(opt_type::OTHER), "-cmd", "--cmd-line",
//			"show the command line");
	this->add_switch(get_opt_index(opt_type::OTHER), "-all", "--all",
			"print output, including states and visible states, in each round");
	this->add_switch(get_opt_index(opt_type::OTHER), SHORT_VERSION_OPT,
			LONG_VERSION_OPT, "show version information and exit");
}

/**
 * @return version information
 */
string cmd_line::create_version_info() {
	string info = ""; ///
	info ///.append("-----------------------------------------------------------\n") ///
	.append("* *       _/_/_/    _/    _/    _/_/_/_/      _/_/_/    * *\n") ///
	.append("* *     _/         _/    _/    _/     _/    _/    _/    * *\n") ///
	.append("* *    _/         _/    _/    _/_/_/_/     _/_/_/_/     * *\n") ///
	.append("* *    _/        _/    _/    _/     _/    _/    _/      * *\n") ///
	.append("* *    _/_/_/    _/_/_/     _/_/_/_/     _/    _/  " + VERSION  ///
			+ " * *\n") ///
	.append("-----------------------------------------------------------\n") ///
	.append("* *    Interprocedural Context-Unbounded Analysis for   * *\n") ///
	.append("* *               Concurrent Programs                   * *\n") ///
	.append("* *          <Anonymous> @ <Anonymous>'s Group          * *\n") ///
	.append("* *                  <Anonymous>                        * *\n") ///
//	.append("* *                            Build Date @ ").append(
//	__DATE__).append(" * *\n")
	.append("-----------------------------------------------------------\n");
	return info;
}
}
