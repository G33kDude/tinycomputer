#include "labels.h"

namespace labels {
	lines::lines find_labels(lines::lines &input_lines, labels &found_labels) {
		lines::lines found_lines;
		int i = 0;
		for (lines::line current_line: input_lines) {
			// If is label definition
			std::string endchar = current_line.text.substr(
					current_line.text.length()-1);
			if (endchar == ":")
			{
				std::string labelname = current_line.text.substr(
						0, current_line.text.length()-1);
				found_labels[labelname] = i;
				continue;
			}
			found_lines.push_back(current_line);
			i++;
		}
		return found_lines;
	}
}
