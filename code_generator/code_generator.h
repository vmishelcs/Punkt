#ifndef CODE_GENERATOR_H_
#define CODE_GENERATOR_H_

#include <parse_node/parse_node_visitor.h>

#include <filesystem>

namespace fs = std::filesystem;

class CodeGenerator {
 public:
  // Writes LLVM IR generated from the given decorated abstract syntax tree to
  // the specified file. If the `output_file_path` argument is empty, IR is
  // output to `stderr`.
  static void WriteIR(std::unique_ptr<ParseNode> decorated_ast,
                      fs::path output_file_path = fs::path());

 private:
  CodeGenerator(std::unique_ptr<ParseNode> decorated_ast,
                fs::path output_file_path);

  void WriteIRToSTDOUT();
  void WriteIRToOutputFile();

  std::unique_ptr<ParseNode> decorated_ast;
  fs::path output_file_path;
};

#endif  // CODE_GENERATOR_H_
