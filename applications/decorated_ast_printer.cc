#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>
#include <utilities/xml_generator_visitor.h>

#include <cassert>

static void CheckForErrors() {
  if (PunktLogger::ThereAreCompileErrors()) {
    PunktLogger::DumpCompileErrorSummary();
    std::cerr << "program has errors." << std::endl;
    std::exit(0);
  }
}

std::unique_ptr<ParseNode> AnalyzeFile(fs::path file_path) {
  std::unique_ptr<ParseNode> ast = Parser::Parse(file_path);
  assert(ast != nullptr && "Parser::Parse returned nullptr");
  CheckForErrors();

  std::unique_ptr<ParseNode> decorated_ast =
      SemanticAnalyzer::Analyze(std::move(ast));
  assert(decorated_ast != nullptr &&
         "SemanticAnalyzer::Analyze returned nullptr");
  CheckForErrors();

  return decorated_ast;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Decorated AST printer application requires an input file "
                 "argument\n";
    return 1;
  }

  fs::path input_file_directory = fs::path(INPUT_FILE_DIRECTORY);
  fs::path file_path = input_file_directory / argv[1];

  auto decorated_ast = AnalyzeFile(file_path);

  if (PunktLogger::ThereAreCompileErrors()) {
    PunktLogger::DumpCompileErrorSummary();
  }

  XMLGeneratorVisitor xml_visitor(std::cerr);
  decorated_ast->Accept(xml_visitor);
}
