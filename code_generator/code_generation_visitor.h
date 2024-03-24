#ifndef CODE_GENERATION_VISITOR_H_
#define CODE_GENERATION_VISITOR_H_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <parse_node/parse_nodes/all_nodes.h>

class CodeGenerationVisitor {
public:
    CodeGenerationVisitor();

    std::string DumpLLVMIR();

    // Non-leaf nodes
    void VisitEnter(CodeBlockNode& node);
    void VisitLeave(CodeBlockNode& node);

    llvm::Value *GenerateCode(DeclarationStatementNode& node);

    void VisitEnter(MainNode& node);
    void VisitLeave(MainNode& node);

    void VisitEnter(OperatorNode& node);
    void VisitLeave(OperatorNode& node);

    void VisitEnter(PrintStatementNode& node);
    void VisitLeave(PrintStatementNode& node);

    void VisitEnter(ProgramNode& node);
    void VisitLeave(ProgramNode& node);

    // Leaf nodes
    llvm::Value *GenerateCode(ErrorNode& node);
    llvm::Value *GenerateCode(IdentifierNode& node);
    llvm::Value *GenerateCode(IntegerLiteralNode& node);

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
};

#endif // CODE_GENERATION_VISITOR_H_
