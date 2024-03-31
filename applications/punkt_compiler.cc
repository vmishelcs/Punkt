#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "error: no input file";
		return 1;
	}

    // std::cout << "Current path: " << fs::current_path() << '\n';
}
