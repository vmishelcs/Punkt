#include "code_generation_visitor.h"
#include "code_generator.h"

#include <iostream>

void CodeGenerator::GenerateIR(std::unique_ptr<ParseNode> decorated_ast) {
    CodeGenerator code_generator(std::move(decorated_ast));
    code_generator.BeginCodeGeneration();
}

CodeGenerator::CodeGenerator(std::unique_ptr<ParseNode> decorated_ast) 
    : decorated_ast(std::move(decorated_ast))
{}

void CodeGenerator::BeginCodeGeneration() {
    CodeGenerationVisitor code_generation_visitor;
    decorated_ast->GenerateCode(code_generation_visitor);
    std::cout << code_generation_visitor.DumpLLVMIR() << '\n';
}
