#pragma once

#include <algorithm>
#include <array>
#include <vector>
#include "lines.h"
#include "labels.h"

#define ASM_BIT_PTR 1 << 2


#define ASM_TYPE_LITRL 1 << 0
#define ASM_TYPE_RWRAM 1 << 1
#define ASM_TYPE_RORAM 1 << 2
#define ASM_TYPE_LABEL 1 << 3

namespace assemble {
	using instruction = std::array<unsigned short, 4>;
	using instructions = std::vector<instruction>; 
	instructions assemble(
			lines::lines &input_lines,
			labels::labels &input_labels);
	instruction parse(lines::line &input_line, labels::labels &input_labels, int i);
}
