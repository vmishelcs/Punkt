#include <parse_node/parse_node.h>
#include <parser/parser.h>

void PrintAST(const std::unique_ptr<ParseNode>& node, int depth = 0) {
    if (node) {
        for (int i = 0; i < depth; ++i) {
            std::cout << '\t';
        }
        std::cout << node->GetNodeString() << std::endl;
        for (const auto& child : node->GetChildren()) {
            PrintAST(child, depth + 1);
        }
    }
}

void ParseFile(std::string file_name) {
    Scanner scanner(file_name);
    std::unique_ptr<ParseNode> ast = Parser::Parse(scanner);
    PrintAST(ast);
}

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "AST printer application requires an input file argument\n";
		return 1;
	}

    std::string input_file_directory = INPUT_FILE_DIRECTORY;
	std::string file_name = input_file_directory + argv[1];
    ParseFile(file_name);
}
