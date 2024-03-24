#ifndef CODE_GENERATION_VISITOR_H_
#define CODE_GENERATION_VISITOR_H_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/ValueSymbolTable.h>

#include <parse_node/parse_nodes/all_nodes.h>

class CodeGenerationVisitor {
public:
    CodeGenerationVisitor();

    std::string DumpLLVMIR();

    // Non-leaf nodes
    llvm::Value *GenerateCode(CodeBlockNode& node);

    llvm::Value *GenerateCode(DeclarationStatementNode& node);

    llvm::Value *GenerateCode(MainNode& node);

    void VisitEnter(OperatorNode& node);
    void VisitLeave(OperatorNode& node);

    void VisitEnter(PrintStatementNode& node);
    void VisitLeave(PrintStatementNode& node);

    llvm::Value *GenerateCode(ProgramNode& node);

    // Leaf nodes
    llvm::Value *GenerateCode(ErrorNode& node);
    llvm::Value *GenerateCode(IdentifierNode& node);
    llvm::Value *GenerateCode(IntegerLiteralNode& node);

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    llvm::ValueSymbolTable value_symbol_table;
};

#endif // CODE_GENERATION_VISITOR_H_
