#include "code_generator.h"

#include <fcntl.h>
#include <logging/punkt_logger.h>

#include "code_generation_visitor.h"

void CodeGenerator::WriteIR(std::unique_ptr<ParseNode> decorated_ast,
                            fs::path output_file_path) {
  CodeGenerator code_generator(std::move(decorated_ast), output_file_path);
  code_generator.WriteIRToOutputFile();
}

CodeGenerator::CodeGenerator(std::unique_ptr<ParseNode> decorated_ast,
                             fs::path output_file_path)
    : decorated_ast(std::move(decorated_ast)),
      output_file_path(output_file_path) {}

void CodeGenerator::WriteIRToOutputFile() {
  ProgramNode& program_node = dynamic_cast<ProgramNode&>(*decorated_ast);
  CodeGenerationVisitor code_generation_visitor(program_node.GetModuleID());

  decorated_ast->GenerateCode(code_generation_visitor);

  if (output_file_path.empty()) {
    code_generation_visitor.WriteIRToFD(STDERR_FILENO);
    return;
  }

  // Create output file
  int output_fd =
      open(output_file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0660);
  if (output_fd == -1) {
    PunktLogger::LogFatal("unable to open output file " +
                          output_file_path.string() +
                          " errno: " + strerror(errno));
  }

  code_generation_visitor.WriteIRToFD(output_fd);

  close(output_fd);
}
