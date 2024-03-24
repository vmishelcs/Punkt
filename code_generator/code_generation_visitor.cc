#include "code_generation_visitor.h"

CodeGenerationVisitor::CodeGenerationVisitor()
    : context(std::make_unique<llvm::LLVMContext>())
    /* TODO: ModuleID should probably be the input source code file name */
    , module(std::make_unique<llvm::Module>("program_module", *context))
    , builder(std::make_unique<llvm::IRBuilder<>>(*context))
{

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

void CodeGenerationVisitor::VisitEnter(DeclarationStatementNode& node) {
    
}
void CodeGenerationVisitor::VisitLeave(DeclarationStatementNode& node) {
    
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
void CodeGenerationVisitor::Visit(ErrorNode& node) {
    
}
void CodeGenerationVisitor::Visit(IdentifierNode& node) {
    
}
void CodeGenerationVisitor::Visit(IntegerLiteralNode& node) {
    
}

llvm::Value *CodeGenerationVisitor::GenerateCode(IntegerLiteralNode& node) {
    return llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), node.GetValue());
}
