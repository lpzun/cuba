/**
 * @brief pds.cc
 *
 * @date  : Aug 30, 2016
 * @author: Peizun Liu
 */

#include "cuba.hh"

namespace cuba {

/**
 * Read and parse CPDS from file
 * @param filename
 */
concurrent_pushdown_automata parser::parse_input_cpds(const string& filename) {
	if (filename == "X")
		throw cuba_runtime_error("Please assign the input file!");

	/// original input file, possibly with comments
	ifstream org_in(filename.c_str());
	if (!org_in.good())
		throw cuba_runtime_error("Input file does not exist!");
	remove_comments(org_in, "/tmp/tmp.pds.no_comment", prop::COMMENT);
	org_in.close();

	ifstream new_in("/tmp/tmp.pds.no_comment");
	/// read the set of control states
	if (!new_in.good())
		throw cuba_runtime_error(
				"Something wrong when reading the input file!");

	/// parse the set of control states: MUST be in the first line
	new_in >> thread_state::S;
	set<pda_state> states;
	for (auto s = 0; s < thread_state::S; ++s) {
		states.emplace(s);
	}

	vector<vector<string>> sCPDS;
	string line;
	while (std::getline(new_in, line)) {
		if (line == "") /// nothing in the line
			continue;
		if (line.at(0) == 'P') {
			sCPDS.emplace_back(vector<string>());
		}
		sCPDS.back().emplace_back(line);
	}
	new_in.close();

	/// build concurrent pushdown automaton
	concurrent_pushdown_automata CPDA;
	for (auto pda : sCPDS) {
		CPDA.emplace_back(parse_input_pda(states, pda));
	}
	return CPDA;
}

/**
 * To parse the input PDS
 * @param filename
 */
pushdown_automaton parser::parse_input_pda(const set<pda_state>& states,
		const vector<string>& sPDA) {
	if (sPDA.size() == 0)
		return pushdown_automaton();
	/// step 1: current PDA's stack symbols
	set<pda_alpha> alphas;
	{
		istringstream iss(sPDA[0]);
		string pda_mark;
		pda_alpha start, end;
		iss >> pda_mark >> start >> end;
		if (pda_mark != "PDA")
			throw cuba_runtime_error("PDA input format error!");
		/// step 1.1: set up alphabet
		for (auto l = start; l <= end; ++l) {
			alphas.emplace(l);
		}
	}

	/// step 2: current PDA's actions and adjacency list
	vector<pda_trans> actions;
	adj_list PDA;
	id_transition trans_id = 0;
	for (int i = 1; i < sPDA.size(); ++i) {
		/// three types of transition:
		///   PUSH: (s1, l1) -> (s2, l2l3)
		///   POP : (s1, l1) -> (s2, )
		///   OVERWRITE: (s1, l1) -> (s2, l2)
		pda_state s1;  /// source state
		pda_alpha l1;  /// source alpha
		string sep;    /// separator ->
		pda_state s2;  /// destination state
		string l2, l3; /// destination alphabets. Note: using string here

		istringstream iss(sPDA[i]);
		iss >> s1 >> l1 >> sep >> s2 >> l2 >> l3;

		/// source thread state
		thread_state src(s1, l1);
		/// destination thread configuration
		pda_stack W; /// the stack of the destination thread configuration
		if (l3 != "") { /// push operation
			W.push(std::stoi(l3));
			W.push(std::stoi(l2));
			thread_config dst(s2, W);
			actions.push_back(pda_trans(src, dst, type_stack_operation::PUSH));
		} else if (l2 != "") { /// overwrite operation
			W.push(std::stoi(l2));
			thread_config dst(s2, W);
			actions.push_back(
					pda_trans(src, dst, type_stack_operation::OVERWRITE));
		} else { /// pop operation
			thread_config dst(s2, W);
			actions.push_back(pda_trans(src, dst, type_stack_operation::POP));
		}

		PDA[src].emplace_back(trans_id++); /// add transition id to src's map
	}
	return pushdown_automaton(states, alphas, actions, PDA);
}

/**
 * Parse the input transitions
 * @param r
 */
void parser::parse_transition(const string& r) {

}

/**
 * To parse the input initial/final thread state
 * @param s
 * @return thread state
 */
thread_config parser::parse_input_cfg(const string& s) {
	if (s.find('|') == std::string::npos) { /// s is store in a file
		ifstream in(s.c_str());
		if (in.good()) {
			string content;
			std::getline(in, content);
			in.close();
			return parser::create_thread_state_from_str(content);
		} else {
			throw cuba_runtime_error(
					"parse_input_SS: input state file is unknown!");
		}
	}
	return parser::create_thread_config_from_str(s);
}

/**
 *
 * @param s_ts
 * @param delim
 * @return
 */
thread_config parser::create_thread_config_from_str(const string& s_ts,
		const char& delim) {
	const auto& vs_tg = split(s_ts, delim);
	if (vs_tg.size() != 2) {
		throw("The format of thread configuration is wrong!");
	}
	const auto& symbols = split(vs_tg[1], ',');
	pda_stack w;
	for (const auto& s : symbols) {
		w.push(std::stoi(s));
	}
	return thread_config(std::stoi(vs_tg[0]), w);
}

/**
 *
 * @param s_ts
 * @param delim
 * @return bool
 */
thread_state parser::create_thread_state_from_str(const string& s_ts,
		const char& delim) {
	deque<string> vs_ts = split(s_ts, delim);
	if (vs_ts.size() != 2) {
		throw("The format of thread state is wrong.");
	}
	return thread_state(std::stoi(vs_ts[0]), stoi(vs_ts[1]));
}

/**
 * @brief remove the comments of .ttd files
 * @param in
 * @param filename
 * @param comment
 */
void parser::remove_comments(istream& in, const string& filename,
		const string& comment) {
	ofstream out(filename.c_str());
	remove_comments(in, out, comment);
}

/**
 * parse the type of stack operation
 * +: push op
 * -: pop op
 * !: overwrite op
 * @param c
 * @return type_stack_operation
 */
type_stack_operation parser::parse_type_stack_operation(const char& c) {
	switch (c) {
	case '+':
		return type_stack_operation::PUSH;
	case '-':
		return type_stack_operation::POP;
	case '!':
		return type_stack_operation::OVERWRITE;
	default:
		throw cuba_runtime_error("illegal transition!");
	}
}

/**
 * parse the type of synchronization
 * +: fork a new thread
 * ~: broadcast
 * -: normal transition, no synchronization
 * @param c
 * @return type_synchronization
 */
type_synchronization parser::parse_type_synchronization(const char& c) {
	switch (c) {
	case '+':
		return type_synchronization::FORK;
	case '~':
		return type_synchronization::BRCT;
	case '-':
		return type_synchronization::NORM;
	default:
		throw cuba_runtime_error("illegal transition!");
	}
}

/**
 * @brief remove the comments of .ttd files
 * @param in
 * @param out
 * @param comment
 */
void parser::remove_comments(const string& in, string& out,
		const string& comment) {
	std::istringstream instream(in);
	std::ostringstream outstream;
	remove_comments(instream, outstream, comment);
	out = outstream.str();
}

/**
 * remove comments
 * @param in
 * @param out
 * @param comment
 */
void parser::remove_comments(istream& in, ostream& out, const string& comment) {
	string line;
	while (getline(in, line = "")) {
		auto comment_start = line.find(comment);
		out
				<< (comment_start == string::npos ?
						line : line.substr(0, comment_start)) << endl;
	}
}

/**
 *
 * @param in
 * @param line
 * @param eol
 * @return bool
 */
bool parser::getline(istream& in, string& line, const char& eol) {
	char c = 0;
	while (in.get(c) ? c != eol : false)
		line += c;
	return c != 0;
}

/**
 * @brief split a string into a vector of strings via a delimiter
 * @param s: object string
 * @param delim: a delimiter
 * @return a vector of string
 */
deque<string> parser::split(const string &s, const char& delim) {
	deque<string> elems;
	istringstream ss(s);
	string item;
	while (std::getline(ss, item, delim)) {
		elems.emplace_back(item);
	}
	return elems;
}

} /* namespace cuba */
