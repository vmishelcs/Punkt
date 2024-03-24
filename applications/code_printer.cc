#include <cassert>

#include <parse_node/parse_node.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>

void GenerateIRCode(std::string file_name) {
	Scanner scanner(file_name);
    std::unique_ptr<ParseNode> ast = Parser::Parse(scanner);
	assert(ast != nullptr && "Parser::Parse returned nullptr");

	std::unique_ptr<ParseNode> decorated_ast = SemanticAnalyzer::Analyze(std::move(ast));
	assert(decorated_ast != nullptr && "SemanticAnalyzer::Analyze returned nullptr");
}

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "Code printer application requires an input file argument\n";
		return 1;
	}

    std::string input_file_directory = INPUT_FILE_DIRECTORY;
	std::string file_name = input_file_directory + argv[1];
	GenerateIRCode(file_name);
}
