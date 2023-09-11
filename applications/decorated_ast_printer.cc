#include <iostream>
#include <string>

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "Decorated AST printer application requires an input file argument\n";
		return 1;
	}

    std::string input_file_directory = INPUT_FILE_DIRECTORY;
	std::string file_name = input_file_directory + argv[1];
}
