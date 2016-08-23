#include <iostream>
#include <stdio.h>
#include <array>
#include <vector>
#include <string>

using instruction = std::array<unsigned short, 4>;
using instructions = std::vector<instruction>;

unsigned short le16_from_bytes(char bytes[]) {
	return bytes[0] | bytes[1] << 8;
}

void print_instruction(instruction cur_instruction) {
	std::vector<std::string> opcodes = {"end", "add", "sub",
		"mul", "div", "mod", "and", "or", "xor",
		"rs", "ls", "not", "beq", "jmp", "vsync",
		"jrt", "ret", "bgt", "bne"};
	int opcode = cur_instruction[0] & 0x7F;
	if (opcode >= opcodes.size()) {
		throw std::runtime_error("Invalid opcode");
	}
	std::cout << opcodes[opcode] << " " <<
		cur_instruction[1] << " " <<
		cur_instruction[2] << " " <<
		cur_instruction[3] << std::endl;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		throw std::runtime_error("Invalid parameter count");
	}

	instructions code;
	FILE* pFile = fopen(argv[1], "rb");
	for (int ip=0; ; ip++) {
		char bytes[8];
		int readbytes = fread(bytes, 1, 8, pFile);
		if (ferror(pFile))
			throw std::runtime_error("Error reading file");
		else if (readbytes == 0) // Reached end of file gracefully
			break;
		else if (readbytes < 8) // Reached end of file with leftover data
			throw std::runtime_error("File size not multiple of 64 bits");
		instruction cur_inst;
		for (int i=0; i < 4; i++) {
			cur_inst[i] = le16_from_bytes(bytes+(i*2));
		}
		code.push_back(cur_inst);
	}
	fclose(pFile);
	
	for (instruction cur_inst: code) {
		print_instruction(cur_inst);
	}

	return 0;
}
