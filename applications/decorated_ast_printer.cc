#include <parse_node/parse_node.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>

void PrintDecoratedAST(const std::unique_ptr<ParseNode>& node, int depth = 0) {
	if (node) {
        for (int i = 0; i < depth; ++i) {
            std::cout << "    ";
        }

        std::cout << node->GetNodeString();
		if (node->HasType()) {
			std::cout << ' ' << node->GetType().GetTypeString();
		}
		if (node->HasScope()) {
			std::cout << ' ' << node->GetScope().GetScopeTypeString();
		}
		std::cout << std::endl;

        for (const auto& child : node->GetChildren()) {
            PrintDecoratedAST(child, depth + 1);
        }
    }
}

void AnalyzeFile(std::string file_name) {
	Scanner scanner(file_name);
    std::unique_ptr<ParseNode> ast = Parser::Parse(scanner);
	std::unique_ptr<ParseNode> decorated_ast = SemanticAnalyzer::Analyze(std::move(ast));
    PrintDecoratedAST(decorated_ast);
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
