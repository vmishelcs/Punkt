#include "llvm/TargetParser/Host.h"

#include "code_generation_visitor.h"


CodeGenerationVisitor::CodeGenerationVisitor()
    : context(std::make_unique<llvm::LLVMContext>())
    /* TODO: ModuleID should probably be the input source code file name */
    , module(std::make_unique<llvm::Module>("program_module", *context))
    , builder(std::make_unique<llvm::IRBuilder<>>(*context))
{
    std::string target_triple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(target_triple);
}

std::string CodeGenerationVisitor::DumpLLVMIR() {
    std::string output;
    llvm::raw_string_ostream ir_ostream(output);
    module->print(ir_ostream, nullptr);
    return ir_ostream.str();
}

// Non-leaf nodes
void CodeGenerationVisitor::VisitEnter(CodeBlockNode& node) {

}
void CodeGenerationVisitor::VisitLeave(CodeBlockNode& node) {

}

llvm::Value *CodeGenerationVisitor::GenerateCode(DeclarationStatementNode& node) {
    // Generate a binding for the value we want assigned to a variable
    // llvm::Value *binding = node.GetChild(1).GenerateCode(*this);

    return nullptr;
}

void CodeGenerationVisitor::VisitEnter(MainNode& node) {
    
}
void CodeGenerationVisitor::VisitLeave(MainNode& node) {
    
}

void CodeGenerationVisitor::VisitEnter(OperatorNode& node) {
    
}
void CodeGenerationVisitor::VisitLeave(OperatorNode& node) {
    
}

void CodeGenerationVisitor::VisitEnter(PrintStatementNode& node) {
    
}
void CodeGenerationVisitor::VisitLeave(PrintStatementNode& node) {
    
}

void CodeGenerationVisitor::VisitEnter(ProgramNode& node) {
    
}
void CodeGenerationVisitor::VisitLeave(ProgramNode& node) {
    
}

// Leaf nodes
llvm::Value *CodeGenerationVisitor::GenerateCode(ErrorNode& node) {
    return nullptr;
}
llvm::Value *CodeGenerationVisitor::GenerateCode(IdentifierNode& node) {
    return nullptr;
}
llvm::Value *CodeGenerationVisitor::GenerateCode(IntegerLiteralNode& node) {
    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), node.GetValue());
}
