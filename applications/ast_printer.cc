#include <cassert>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parser/parser.h>

void PrintAST(const ParseNode& node, int depth = 0) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "    ";
    }
    std::cout << node.ToString() << std::endl;
    for (const auto& child : node.GetChildren()) {
        PrintAST(child, depth + 1);
    }
}

void ParseFile(fs::path file_path) {
    std::unique_ptr<ParseNode> ast = Parser::Parse(file_path);
    assert(ast != nullptr && "Parser::Parse returned nullptr");
    PrintAST(*ast);
}

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "AST printer application requires an input file argument\n";
		return 1;
	}

    fs::path input_file_directory = fs::path(INPUT_FILE_DIRECTORY);
	fs::path file_path = input_file_directory / argv[1];
    ParseFile(file_path);
    PunktLogger::DumpCompileErrors(stderr);
}
