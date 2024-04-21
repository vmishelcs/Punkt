#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parser/parser.h>
#include <utilities/xml_generator_visitor.h>

#include <fstream>
#include <iostream>

std::unique_ptr<ParseNode> ParseFile(fs::path file_path) {
  std::unique_ptr<ParseNode> ast = Parser::Parse(file_path);
  assert(ast != nullptr && "Parser::Parse returned nullptr");
  return ast;
}

bool ThereAreErrors() { return PunktLogger::ThereAreCompileErrors(); }

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "AST XML printer application requires input and output file "
                 "arguments"
              << std::endl;
    return 1;
  }

  fs::path input_file_path = fs::path(argv[1]);
  fs::path output_file_path = fs::path(argv[2]);

  auto ast = ParseFile(input_file_path);

  if (ThereAreErrors()) {
    std::cerr << "program has errors. no output file generator" << std::endl;
    return 1;
  }

  std::ofstream output_file_stream(output_file_path,
                                   std::ios_base::trunc | std::ios_base::out);
  XMLGeneratorVisitor xml_visitor(output_file_stream);

  ast->Accept(xml_visitor);

  output_file_stream.close();
  return 0;
}
