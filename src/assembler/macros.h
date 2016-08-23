#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstring> // strlen
#include "lines.h"

#define MACRO_START "!@"
#define MACRO_END "!"
#define MACRO_REF "@"

namespace macros {
	struct macro {
		std::string name;
		std::vector<lines::line> repl;
	};
	using macros = std::map<std::string,macro>;
	lines::lines process_macros(lines::lines &lines);
	lines::lines find_macros(
			lines::lines &lines,
			std::map<std::string,macro> &macros);
	lines::lines replace_macros(
			lines::lines &input_lines,
			macros &input_macros);
	void split_macro(
			std::string &macrogtext,
			std::string &macroname,
			std::string &macrorepl);
}
