#include "code_generation_visitor.h"
#include "code_generator.h"

#include <iostream>

void CodeGenerator::GenerateIR(std::string file_name, std::unique_ptr<ParseNode> decorated_ast) {
    CodeGenerator code_generator(file_name, std::move(decorated_ast));
    code_generator.BeginCodeGeneration();
}

CodeGenerator::CodeGenerator(std::string file_name, std::unique_ptr<ParseNode> decorated_ast) 
    : file_name(file_name)
    , decorated_ast(std::move(decorated_ast))
{}

void CodeGenerator::BeginCodeGeneration() {
    CodeGenerationVisitor code_generation_visitor(file_name);
    decorated_ast->GenerateCode(code_generation_visitor);
    std::cout << code_generation_visitor.DumpLLVMIR() << '\n';
}
