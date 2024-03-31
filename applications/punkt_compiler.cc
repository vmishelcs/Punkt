#include <filesystem>
#include <iostream>
#include <sys/stat.h>

#include <code_generator/code_generator.h>
#include <logging/punkt_logger.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>

static fs::path GetFilePath(const char *input) {
	fs::path result{input};
	if (!fs::exists(result)) {
		PunktLogger::LogFatal("file " + result.string() + " does not exist");
	}
	return result;
}

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "error: no input file";
		return 1;
	}

	fs::path input_file_path = GetFilePath(argv[1]);
}
