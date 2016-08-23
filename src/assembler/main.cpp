#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <stdio.h>
#include "lines.h"
#include "macros.h"
#include "labels.h"
#include "assemble.h"

void write_uint16_be(FILE * pFile, unsigned short &ushort16) {
	char buffer[] = {(char)(ushort16 & 0xFF), (char)((ushort16 >> 8) & 0xFF)};
	fwrite(buffer, sizeof(char), sizeof(buffer), pFile);
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "Invalid call\n";
		return 1;
	}

	lines::lines tinycode = lines::lines_from_cin();
	lines::lines second = lines::clean_lines(tinycode);
	lines::lines third = macros::process_macros(second);
	lines::lines fourth = lines::clean_lines(third);
	labels::labels tinylabels;
	lines::lines fifth = labels::find_labels(fourth, tinylabels);
	assemble::instructions instructions = assemble::assemble(
			fifth, tinylabels);

	FILE * pFile = fopen(argv[1], "wb");
	for (assemble::instruction current_instruction: instructions) {
		for (auto ushort16: current_instruction) {
			write_uint16_be(pFile, ushort16);
		}
	}
	fclose(pFile);

	return 0;
}

