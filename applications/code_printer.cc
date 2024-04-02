#include <cassert>

#include <parse_node/parse_node.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>
#include <code_generator/code_generator.h>

void GenerateIRCode(fs::path file_path) {
    std::unique_ptr<ParseNode> ast = Parser::Parse(file_path);
	assert(ast != nullptr && "Parser::Parse returned nullptr");

	std::unique_ptr<ParseNode> decorated_ast = SemanticAnalyzer::Analyze(std::move(ast));
	assert(decorated_ast != nullptr && "SemanticAnalyzer::Analyze returned nullptr");

	CodeGenerator::WriteIR(std::move(decorated_ast));
}

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "Code printer application requires an input file argument\n";
		return 1;
	}

    fs::path input_file_directory = fs::path(INPUT_FILE_DIRECTORY);
	fs::path file_path = input_file_directory / argv[1];
	GenerateIRCode(file_path);
}
