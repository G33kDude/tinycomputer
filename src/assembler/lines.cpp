#include "lines.h"

namespace lines {

lines lines_from_cin() {
	lines input_lines;
	int i = 0;
	std::string linetext;
	while (std::getline(std::cin, linetext)) {
		line current_line;
		current_line.number = i;
		current_line.text = linetext;
		input_lines.push_back(current_line);
		i++;
	}
	return input_lines;
}

lines clean_lines(lines &input_lines) {
	lines cleaned_lines;
	for (line current_line: input_lines) {
		size_t found = current_line.text.find(COMMENT_CHAR);
		if (found != std::string::npos)
			current_line.text = current_line.text.substr(0, found);
		trim_line(current_line);
		if (current_line.text != "")
			cleaned_lines.push_back(current_line);
	}
	return cleaned_lines;
}

void trim_line(line &input) {
	size_t start = input.text.find_first_not_of(WHITESPACE);
	if (start == std::string::npos) {
		input.text = "";
		return;
	}
	size_t end = input.text.find_last_not_of(WHITESPACE);
	input.text = input.text.substr(start, end-start+1);
}

std::vector<std::string> split_line(line input) { // Not by reference
	std::vector<std::string> elements;
	size_t found = input.text.find_first_of(WHITESPACE);
	while (found != std::string::npos) {
		std::string element = input.text.substr(0, found);
		elements.push_back(element);
		found = input.text.find_first_not_of(WHITESPACE, found);
		input.text = input.text.substr(found);
		found = input.text.find_first_of(WHITESPACE);
	}
	elements.push_back(input.text);
	return elements;
}

}
