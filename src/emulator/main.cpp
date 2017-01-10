#include <iostream>
#include <stdio.h>
#include <array>
#include <vector>
#include <string>
#include <curses.h>

using instruction = std::array<unsigned short, 4>;
using instructions = std::vector<instruction>;
std::array<int, 0xFFFF> global_ram;
std::array<int, 0xFFFF> global_rom;
std::vector<int> callstack;
#include <chrono>
#include <thread>

unsigned short le16_from_bytes(unsigned char bytes[]) {
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

int get_param(instruction instr, int param) {
	int meta = (instr[0] >> (4+param*3)) & 7; // 0b111
	int value = instr[param];
	if (meta & 2) {
		value = global_rom[value];
	} else if (meta & 1) {
		value = global_ram[value];
	}
	if (meta & 4) {
		value = global_ram[value];
	}
	return value;
}

void set_param(instruction instr, int param, int value) {
	int meta = (instr[0] >> (4+param*3)) & 7; // 0b111
	if (meta == 0) {
		throw std::runtime_error("Attempting to set integer literal");
	}
	if (meta & 4) { // 0b100
		int dest = instr[param];
		if (meta & 2) {
			dest = global_rom[dest];
		} else if (meta & 1) {
			dest = global_ram[dest];
		}
		global_ram[dest] = value;
	} else {
		if (meta & 2) {
			throw std::runtime_error("Attemting to set ROM");
		} else if (meta & 1) {
			global_ram[instr[param]] = value;
		} else {
			printf("%i", meta);
			throw std::runtime_error("Unkown state");
		}
	}
}

void vsync() {
	for (int y=0; y<32; y++) {
		for (int x=0; x<32; x++) {
			if ((unsigned int)global_ram[y] & (1 << x)) {
				mvwaddch(stdscr, y, x, '#');
			} else {
				mvwaddch(stdscr, y, x, ' ');
			}
		}
	}
	int btn = 0;
	int ch;
	while ((ch = getch()) != ERR) {
		switch (ch) {
			case KEY_UP:
				btn |= 16;
				break;
			case KEY_DOWN:
				btn |= 64;
				break;
			case KEY_LEFT:
				btn |= 32;
				break;
			case KEY_RIGHT:
				btn |= 128;
				break;
		}
	}
	global_rom[0] = btn;
	wrefresh(stdscr);
	std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

int do_instr(instruction instr, int ip) {
	int opcode = instr[0] & 0x7F;
	switch (opcode) {
		case 1: // add
			set_param(instr, 3, get_param(instr, 1) + get_param(instr, 2));
			break;
		case 2: // sub
			set_param(instr, 3, get_param(instr, 1) - get_param(instr, 2));
			break;
		case 3: // mul
			set_param(instr, 3, get_param(instr, 1) * get_param(instr, 2));
			break;
		case 4: // div
			set_param(instr, 3, get_param(instr, 1) / get_param(instr, 2));
			break;
		case 5: // mod
			set_param(instr, 3, get_param(instr, 1) % get_param(instr, 2));
			break;
		case 6: // and
			set_param(instr, 3, get_param(instr, 1) & get_param(instr, 2));
			break;
		case 7: // or
			set_param(instr, 3, get_param(instr, 1) | get_param(instr, 2));
			break;
		case 8: // xor
			set_param(instr, 3, get_param(instr, 1) ^ get_param(instr, 2));
			break;
		case 9: // rs
			set_param(instr, 3, get_param(instr, 1) >> get_param(instr, 2));
			break;
		case 10: // ls
			set_param(instr, 3, get_param(instr, 1) << get_param(instr, 2));
			break;
		case 11: // not
			set_param(instr, 2, ~get_param(instr, 1));
			break;
		case 12: // beq
			if (get_param(instr, 1) == get_param(instr, 2)) {
				ip += (short)get_param(instr, 3);
			}
			break;
		case 13: // jmp
			ip += (short)get_param(instr, 1);
			break;
		case 14: // vsync
			vsync();
			break;
		case 15: // jrt
			callstack.push_back(ip);
			ip += (short)get_param(instr, 1);
			break;
		case 16: // ret
			ip = callstack.back();
			callstack.pop_back();
			break;
		case 17: // bgt
			if (get_param(instr, 1) > get_param(instr, 2)) {
				ip += (short)get_param(instr, 3);
			}
			break;
		case 18: // bne
			if (get_param(instr, 1) != get_param(instr, 2)) {
				ip += (short)get_param(instr, 3);
			}
			break;
			
	}
//	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	return ip;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		throw std::runtime_error("Invalid parameter count");
	}

	instructions code;
	FILE* pFile = fopen(argv[1], "rb");
	for (;;) {
		unsigned char bytes[8];
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
	
	initscr();
	cbreak();
	noecho();
//	start_color();
//	init_pair(1, COLOR_BLACK, COLOR_WHITE);
//	wbkgd(stdscr, COLOR_PAIR(1));
	curs_set(0);
	keypad(stdscr, TRUE); // Enable getch() special chars, such as arrows
	nodelay(stdscr, TRUE); // Disable getch() blocking
	
	printf("code size: %i\n", (int)code.size());
	for (int ip=0; ip<code.size(); ) {
		ip = do_instr(code[ip], ip+1);
	}
	
	int x = 16; int y = 16;

	int ch;
	unsigned char btn = 0;
	while (1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	return 0;
}
