#include <cassert>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>

void PrintDecoratedAST(ParseNode& node, int depth = 0) {
	for (int i = 0; i < depth; ++i) {
		std::cout << "    ";
	}

	std::cout << node.ToString();
	if (node.HasType()) {
		std::cout << ' ' << node.GetType().ToString();
	}
	if (node.HasScope()) {
		std::cout << ' ' << node.GetScope().ToString();
	}
	std::cout << '\n';

	for (const auto& child : node.GetChildren()) {
		PrintDecoratedAST(child, depth + 1);
	}
}

void AnalyzeFile(fs::path file_path) {
    std::unique_ptr<ParseNode> ast = Parser::Parse(file_path);
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

    fs::path input_file_directory = fs::path(INPUT_FILE_DIRECTORY);
	fs::path file_path = input_file_directory / argv[1];
	AnalyzeFile(file_path);
	PunktLogger::DumpCompileErrors(stderr);
}
