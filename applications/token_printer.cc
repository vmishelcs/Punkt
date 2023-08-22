#include <iostream>

#include <scanner/scanner.h>

void ScanFile(std::string file_name) {
	Scanner scanner(file_name);
	while (scanner.HasNext()) {
		std::cout << *scanner.Next() << std::endl;
	}
	std::cout << *scanner.Next() << std::endl; // Print EOF token
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Token printer application requires an input file argument\n";
		return 0;
	}

	std::string input_file_directory = INPUT_FILE_DIRECTORY;
	std::string file_name = input_file_directory + argv[1];
	ScanFile(file_name);
}
