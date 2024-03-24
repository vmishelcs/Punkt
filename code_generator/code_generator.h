#ifndef CODE_GENERATOR_H_
#define CODE_GENERATOR_H_

#include <parse_node/parse_node_visitor.h>

class CodeGenerator {
public:
    static void GenerateIR(std::unique_ptr<ParseNode> decorated_ast);

private:
    CodeGenerator(std::unique_ptr<ParseNode> decorated_ast);

    void BeginCodeGeneration();

    std::unique_ptr<ParseNode> decorated_ast;
};

#endif // CODE_GENERATOR_H_
