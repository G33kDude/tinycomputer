#pragma once

#include <map>
#include "lines.h"

namespace labels {
	using labels = std::map<std::string, int>;
	lines::lines find_labels(lines::lines &input_lines, labels &found_labels);
}
