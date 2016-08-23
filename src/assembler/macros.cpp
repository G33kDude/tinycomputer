#include "macros.h"

namespace macros
{

lines::lines process_macros(lines::lines &lines)
{
	lines::lines processed_lines;
	macros macros_map;
	processed_lines = find_macros(lines, macros_map);
	processed_lines = replace_macros(processed_lines, macros_map);
	return processed_lines;
}

// TODO: Don't output blank lines
lines::lines replace_macros(
		lines::lines &input_lines,
		macros &input_macros) {
	lines::lines replaced_lines;

	// TODO: Maximum replacement limit
	for (lines::line current_line: input_lines) { // Not by ref
		// Find references
		size_t macro_start = current_line.text.find(MACRO_REF);
		while (macro_start != std::string::npos) {
			// Find end of reference (whitespace or line end)
			size_t macro_end = current_line.text.find_first_of(
					WHITESPACE, macro_start);
			if (macro_end == std::string::npos)
				macro_end = current_line.text.length();

			// Split out macro name
//			std::cout << "line: " << current_line.text << "\n";
//			std::cout << "start: " << macro_start << "\n";
//			std::cout << "end: " << macro_end << "\n";
			std::string macroname = current_line.text.substr(
					macro_start, macro_end-macro_start);
//			std::cout << "name: " << macroname << "\n";
			macroname = macroname.substr(1);
//			std::cout << "name: " << macroname << "\n";

			// Verify macro name exists
			if (input_macros.count(macroname) == 0) {
				throw std::runtime_error("Unknown macro name on line " +
						std::to_string(current_line.number) +
						": " + macroname);
			}

			// Get macro for replacement
			macro current_macro = input_macros[macroname];
//			std::cout << "target replacement lines: " << current_macro.repl.size() << "\n";

			// Recursively replace any macros in the macro replacement text
			lines::lines macro_repl = replace_macros(
					current_macro.repl, input_macros);

			// Replace line(s)
			lines::line replaced_line;
			replaced_line.text = current_line.text.substr(0, macro_start);
			for (lines::line repl_line: macro_repl) {
				replaced_line.number = current_line.number;
				replaced_line.text += repl_line.text;
				replaced_lines.push_back(replaced_line);
//				std::cout << "line: " << replaced_line.text << "\n";
				replaced_line = {};
			}
//			std::cout << "---\n";
			replaced_line = replaced_lines.back();
			replaced_lines.pop_back();
			replaced_line.text += current_line.text.substr(macro_end);
			current_line = replaced_line;

			macro_start = current_line.text.find(MACRO_REF);
		}
		replaced_lines.push_back(current_line);
	}

	return replaced_lines;
}

lines::lines find_macros(
		lines::lines &input_lines,
		macros &found_macros) {
	lines::lines found_lines;
	macro current_macro;
	for (lines::line current_line: input_lines) {
		// In the middle of a multi-line macro
		if (current_macro.name != "")
		{
			// If not yet to the end of the macro definition
			size_t macro_end = current_line.text.find(MACRO_END);
			if (macro_end == std::string::npos)
			{
				current_macro.repl.push_back(current_line);
				continue;
			}

			// Save line to macro
			lines::line repl = current_line; // Copy line (number)
			repl.text = current_line.text.substr(0, macro_end);
			if (repl.text != "")
				current_macro.repl.push_back(repl);

			// Preserve rest of line
			current_line.text = current_line.text.substr(
					macro_end+std::strlen(MACRO_END));
			
			// Save macro
			found_macros[current_macro.name] = current_macro;
			current_macro = {}; // Clear current macro
		}

		// Find new macro definitions
		size_t macro_start = current_line.text.find(MACRO_START);
		while (macro_start != std::string::npos)
		{
			// Get the macro name/replacement text from line
			std::string macrotext;
			macrotext = current_line.text.substr(
					macro_start+std::strlen(MACRO_START));
			current_line.text = current_line.text.substr(0, macro_start);

			// Single-line macro
			size_t macro_end = macrotext.find(MACRO_END);
			if (macro_end != std::string::npos)
			{
				// Preserve rest of line
				current_line.text += macrotext.substr(
						macro_end+std::strlen(MACRO_END));
				macrotext = macrotext.substr(0, macro_end);
			}

			// Save line to macro
			current_macro = {};
			lines::line repl = current_line; // Copy line (number)
			split_macro(macrotext, current_macro.name, repl.text);
			lines::trim_line(repl);
			if (repl.text != "")
				current_macro.repl.push_back(repl);

			// Single-line macro
			if (macro_end != std::string::npos)
			{
				// Save macro
				found_macros[current_macro.name] = current_macro;
				current_macro = {}; // Clear current macro
			}

			macro_start = current_line.text.find(MACRO_START);
		}
		
		// Save the remnants of the original line
		if (current_line.text != "")
			found_lines.push_back(current_line);
	}
	return found_lines;
}

void split_macro(
		std::string &macrotext,
		std::string &macroname,
		std::string &macrorepl) {
	size_t pos_space = macrotext.find_first_of(WHITESPACE);
	if (pos_space != std::string::npos)
	{
		macroname = macrotext.substr(0, pos_space);
		macrorepl = macrotext.substr(pos_space);
	}
	else
	{
		macroname = macrotext;
		macrorepl = "";
	}
}

}
