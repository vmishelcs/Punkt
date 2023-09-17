#include <cassert>

#include <parse_node/parse_node.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>

void PrintDecoratedAST(ParseNode& node, int depth = 0) {
	for (int i = 0; i < depth; ++i) {
		std::cout << "    ";
	}

	std::cout << node.AsString();
	if (node.HasType()) {
		std::cout << ' ' << node.GetType().AsString();
	}
	if (node.HasScope()) {
		std::cout << ' ' << node.GetScope().AsString();
	}
	std::cout << '\n';

	for (const auto& child : node.GetChildren()) {
		PrintDecoratedAST(child, depth + 1);
	}
}

void AnalyzeFile(std::string file_name) {
	Scanner scanner(file_name);
    std::unique_ptr<ParseNode> ast = Parser::Parse(scanner);
	assert(ast != nullptr && "Parser::Parse returned nullptr");
	std::unique_ptr<ParseNode> decorated_ast = SemanticAnalyzer::Analyze(std::move(ast));
	assert(decorated_ast != nullptr && "SemanticAnalyzer::Analyze returned nullptr");
    PrintDecoratedAST(*decorated_ast);
}

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "Decorated AST printer application requires an input file argument\n";
		return 1;
	}

    std::string input_file_directory = INPUT_FILE_DIRECTORY;
	std::string file_name = input_file_directory + argv[1];
	AnalyzeFile(file_name);
}
