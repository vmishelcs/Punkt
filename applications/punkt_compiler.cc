#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "error: no input file";
		return 1;
	}

    std::cout << "Current path: " << std::filesystem::current_path() << '\n';
}
