#ifndef CODE_GENERATOR_H_
#define CODE_GENERATOR_H_

#include <parse_node/parse_node_visitor.h>

#include <filesystem>

namespace fs = std::filesystem;

class CodeGenerator {
 public:
  /// @brief Generate LLVM IR based on the provided decorated AST.
  /// @param decorated_ast Decorated abstract syntax tree representing the
  /// program.
  /// @param output_file_path Optional parameter for IR output file. If no file
  /// path is specified, IR is written to `STDOUT`.
  static void GenerateIR(std::unique_ptr<ParseNode> decorated_ast,
                         fs::path output_file_path = fs::path());

 private:
  CodeGenerator(std::unique_ptr<ParseNode> decorated_ast,
                fs::path output_file_path);

  void WriteIRToOutputFile();

  std::unique_ptr<ParseNode> decorated_ast;
  fs::path output_file_path;
};

#endif  // CODE_GENERATOR_H_
