#include <cassert>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parser/parser.h>
#include <utilities/xml_generator_visitor.h>

std::unique_ptr<ParseNode> ParseFile(fs::path file_path) {
    std::unique_ptr<ParseNode> ast = Parser::Parse(file_path);
    assert(ast != nullptr && "Parser::Parse returned nullptr");
    return ast;
}

int main(int argc, char **argv) {
    if (argc != 2) {
		std::cerr << "AST printer application requires an input file argument\n";
		return 1;
	}

    fs::path input_file_directory = fs::path(INPUT_FILE_DIRECTORY);
	fs::path file_path = input_file_directory / argv[1];

    auto ast = ParseFile(file_path);

    PunktLogger::DumpCompileErrors();

    XMLGeneratorVisitor xml_visitor(std::cerr);
	ast->Accept(xml_visitor);
}
