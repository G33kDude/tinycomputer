#pragma once

#include <iostream>
#include <vector>
#include <string>

#define COMMENT_CHAR "#"
#define WHITESPACE " \t"

namespace lines {
	struct line {
		int number;
		std::string text;
	};
	void trim_line(line &input);
	using lines = std::vector<line>;
	lines lines_from_cin();
	lines clean_lines(lines &lines);
	std::vector<std::string> split_line(line input);
}
