#include <filesystem>
#include <iostream>

#include <code_generator/code_generator.h>
#include <logging/punkt_logger.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>

static const char *GetCmdOption(int argc, char *argv[], const std::string& option);
static fs::path GetInputFilePath(const char *input);
static fs::path GetOutputFilePath(int argc, char *argv[]);
static void Compile(fs::path input_file_path, fs::path output_file_path);
static void GenerateCodeIfNoErrors(fs::path input_file_path,
		std::unique_ptr<ParseNode> decorated_ast, fs::path output_file_path);
static bool ThereAreErrors();

int main(int argc, char *argv[]) {
    if (argc < 2) {
		PunktLogger::LogFatal("no input file");
	}

	fs::path input_file_path = GetInputFilePath(argv[1]);
	fs::path output_file_path = GetOutputFilePath(argc, argv);
	if (output_file_path.empty()) {
		output_file_path = fs::path(input_file_path).replace_extension("ll");
	}

	std::cout << "input file:  " << input_file_path.string() << std::endl;
	std::cout << "output file: " << output_file_path.string() << std::endl;

	Compile(input_file_path, output_file_path);

	return 0;
}

// Obtain argument to command line option 'option'.
// Implementation inspired by https://stackoverflow.com/a/868894.
static const char *GetCmdOption(int argc, char *argv[], const std::string& option) {
	auto it = std::find(argv, argv + argc, option);
	if (it == argv + argc) {
		// This means output option was not specified
		return nullptr;
	}
	if (++it == argv + argc) {
		// This means output option is missing a filename
		PunktLogger::LogFatal("missing filename after \'" + option + "\'");
	}
	return *it;
}

static fs::path GetInputFilePath(const char *input) {
	fs::path input_file_path = fs::absolute(fs::path(input));
	if (!fs::exists(input_file_path)) {
		PunktLogger::LogFatal("file " + input_file_path.string() + " does not exist");
	}
	return input_file_path;
}

static fs::path GetOutputFilePath(int argc, char *argv[]) {
	const char *output_path_str = GetCmdOption(argc, argv, "-o");
	if (!output_path_str) {
		return fs::path();
	}
	
	fs::path output_file_path = fs::absolute(fs::path(output_path_str));

	if (!fs::exists(output_file_path.parent_path())) {
		PunktLogger::LogFatal("directory " + output_file_path.parent_path().string()
				+ " does not exist");
	}

	if (output_file_path.extension() != fs::path("ll")) {
		output_file_path.replace_extension("ll");
	}
	
	return output_file_path;
}

static void Compile(fs::path input_file_path, fs::path output_file_path) {
	auto ast = Parser::Parse(input_file_path);
	auto decorated_ast = SemanticAnalyzer::Analyze(std::move(ast));
	GenerateCodeIfNoErrors(input_file_path, std::move(decorated_ast), output_file_path);
}

static void GenerateCodeIfNoErrors(fs::path input_file_path,
		std::unique_ptr<ParseNode> decorated_ast, fs::path output_file_path)
{
	if (ThereAreErrors()) {
		PunktLogger::DumpCompileErrors(stderr);
		std::cerr << "program has errors. no output file generated.\n";
	}
	else {
		CodeGenerator::WriteIR(input_file_path, std::move(decorated_ast), output_file_path);
	}
}

static bool ThereAreErrors() {
	return PunktLogger::ThereAreCompileErrors();
}
