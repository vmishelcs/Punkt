#ifndef CODE_GENERATOR_H_
#define CODE_GENERATOR_H_

#include <filesystem>

#include <parse_node/parse_node_visitor.h>

namespace fs = std::filesystem;

class CodeGenerator {
public:
    static void WriteIR(std::unique_ptr<ParseNode> decorated_ast, fs::path output_file_path);

private:
    CodeGenerator(std::unique_ptr<ParseNode> decorated_ast, fs::path output_file_path);

    void WriteIRToOutputFile();

    std::unique_ptr<ParseNode> decorated_ast;
    fs::path output_file_path;
};

#endif // CODE_GENERATOR_H_
