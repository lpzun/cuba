/**
 * @brief main.cc
 *
 * @date  : Jul 31, 2017
 * @author: lpzun
 */

#include "aide.hh"
#include "bopp.tab.hh"

using namespace bopp;

/**
 * @brief get a command
 * @param begin
 * @param end
 * @param option
 * @return a cmd
 */
char* getCmdOption(char ** begin, char ** end, const std::string & option) {
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end) {
		return *itr;
	}
	return 0;
}

/**
 * @brief if a specific cmd exists or not
 * @param begin
 * @param end
 * @param option
 * @return true if exists
 *         false otherwise
 */
bool cmdOptionExists(char** begin, char** end, const std::string& option) {
	return std::find(begin, end, option) != end;
}

int main(int argc, char *argv[]) {
	try {
		if (cmdOptionExists(argv, argv + argc, "-h")) {
			printf("Usage: BoPP [-cfg file] [-taf file]\n");
		}
		auto cfg_file_name = getCmdOption(argv, argv + argc, "-cfg");
		if (cfg_file_name == 0) {
			cfg_file_name = "bp.cfg";
		}
		char* taf_file_name = getCmdOption(argv, argv + argc, "-taf");
		if (taf_file_name == 0) {
			taf_file_name = "bp.taf";
		}
		char DEFAULT_CFG_FILE_NAME[100] = "bp.cfg";
		char DEFAULT_TAF_FILE_NAME[100] = "bp.taf";
		/// file list
		FILE *yyin = fopen(cfg_file_name, "r");
		cout << "starting to parse Boolean programs...\n";
		paide aide(mode::POST);
		yy::bp parser(aide);
		/// step 4: run the parser to parse the input BP
		int result = parser.parse();
		if (result != 0) {
			throw runtime_error(
					"Parser exit with exception: " + std::to_string(result));
		}

		/// step 5: output the parsing result...
		cout << "shared, local, line\n";
		refs::SV_NUM = aide.s_vars_num;
		refs::LV_NUM = aide.l_vars_num;
		refs::PC_NUM = aide.lineno;
		cout << refs::SV_NUM << "," << refs::LV_NUM << "," << refs::PC_NUM
				<< "\n";
		cout << "for testing: before...\n";
		aide.print_control_flow_graph();

		return 0;
	} catch (const runtime_error& e) {
		e.what();
	} catch (...) {

	}
}

