#include "assemble.h"

namespace assemble {
	// http://stackoverflow.com/a/8889045/2472322
	bool is_digits(const std::string &str)
	{
			return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
	}

	instructions assemble(
			lines::lines &input_lines,
			labels::labels &input_labels) {
//		std::cout << "assembling\n";
		instructions assembled;
		int i = 0;
		for (lines::line current_line: input_lines) {
//			std::cout << "line " << i << "\n";
//			std::cout << current_line.text << "\n";
			instruction current_instruction;
			current_instruction = parse(current_line, input_labels, i);
//			std::cout << std::bitset<16>(current_instruction[0]) << " ";
//			std::cout << current_instruction[1] << " ";
//			std::cout << current_instruction[2] << " ";
//			std::cout << current_instruction[3] << "\n";
			assembled.push_back(current_instruction);

			i++;
		}
		return assembled;
	}

	instruction parse(lines::line &input_line, labels::labels &input_labels,
			int index) {
		std::map<std::string, std::vector<int>> opcodes = {
			// type is bit field: Label RORAM RWRAM Literal 
			// opcode, paramcount, type,type,type
			{"end", {0,   0, 0, 0, 0}},
			{"add", {1,   3, 7, 7, 2}},
			{"sub", {2,   3, 7, 7, 2}},
			{"mul", {3,   3, 7, 7, 2}},
			{"div", {4,   3, 7, 7, 2}},
			{"mod", {5,   3, 7, 7, 2}},
			{"and", {6,   3, 7, 7, 2}},
			{"or",  {7,   3, 7, 7, 2}},
			{"xor", {8,   3, 7, 7, 2}},
			{"rs",  {9,   3, 7, 7, 2}},
			{"ls",  {10,  3, 7, 7, 2}},
			{"not", {11,  2, 7, 2, 0}},
			{"beq", {12,  3, 7, 7, 8}},
			{"jmp", {13,  1, 8, 0, 0}},
			{"vsync", {14,0, 0, 0, 0}},
			{"jrt", {15,  1, 8, 0, 0}},
			{"ret", {16,  0, 0, 0, 0}},
			{"bgt", {17,  3, 7, 7, 8}},
			{"bne", {18,  3, 7, 7, 8}}
		};
		instruction output = {};
		std::vector<std::string> codes = lines::split_line(input_line);
		std::string opcode = codes[0];
		if (opcodes.find(opcode) == opcodes.end()) {
			throw std::runtime_error("Unkown opcode on line " +
					std::to_string(input_line.number) + ": " + opcode);
		}
		if (codes.size()-1 != opcodes[opcode][1]) {
			throw std::runtime_error("Parameter count mismatch on line " +
					std::to_string(input_line.number));
		}

		output[0] = opcodes[opcode][0];
		for (int i=0; i < codes.size()-1; i++) {
			int metadata = 0;
			std::string param = codes[i+1];

			// If necessary, handle pointer
			if (param.substr(0, 1) == "*") {
				metadata |= ASM_BIT_PTR;
				param = param.substr(1);
			}

			std::string type = param.substr(0, 1);
			std::string data = param.substr(1);

			// TODO: Allow more types for jump
			// TODO: Value range checking
			// Get type and value of parameter
			int ntype; // Numeric type
			unsigned short ndata; // Numeric version of param
			if (((type == "-" || type == "+") && is_digits(data)) ||
					is_digits(param)) {
				ntype = ASM_TYPE_LITRL; ndata = stoi(param);
			} else if ((type == "m" || type == "v") && is_digits(data)) {
				ntype = ASM_TYPE_RWRAM; ndata = stoi(data);
			} else if ((type == "b") && is_digits(data)) {
				ntype = ASM_TYPE_RORAM; ndata = stoi(data);
			} else {
				ntype = ASM_TYPE_LABEL; ndata = input_labels[param] - index;
			}

			// Verify type is valid for parameter
			if (!(ntype & opcodes[opcode][2+i])) {
				throw std::runtime_error("Invalid parameter type on line " +
						std::to_string(input_line.number) +
						" for paramerater " + std::to_string(i+1));
			}

			// Label isn't actually a type, it's a literal in disguise
			if (ntype == ASM_TYPE_LABEL)
				ntype = ASM_TYPE_LITRL;

			metadata |= ntype >> 1; // TODO: More semantic conversion method
			output[0] |= metadata << (i*3 + 7);
			output[i+1] = ndata;
		}

		return output;
	}
}
