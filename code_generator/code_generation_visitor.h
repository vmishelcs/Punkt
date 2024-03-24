#ifndef CODE_GENERATION_VISITOR_H_
#define CODE_GENERATION_VISITOR_H_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <parse_node/parse_node_visitor.h>

class CodeGenerationVisitor : public ParseNodeVisitor {
public:
    CodeGenerationVisitor();

    std::string DumpLLVMIR();

    // Non-leaf nodes
    virtual void VisitEnter(CodeBlockNode& node) override;
    virtual void VisitLeave(CodeBlockNode& node) override;

    virtual void VisitEnter(DeclarationStatementNode& node) override;
    virtual void VisitLeave(DeclarationStatementNode& node) override;

    virtual void VisitEnter(MainNode& node) override;
    virtual void VisitLeave(MainNode& node) override;

    virtual void VisitEnter(OperatorNode& node) override;
    virtual void VisitLeave(OperatorNode& node) override;

    virtual void VisitEnter(PrintStatementNode& node) override;
    virtual void VisitLeave(PrintStatementNode& node) override;

    virtual void VisitEnter(ProgramNode& node) override;
    virtual void VisitLeave(ProgramNode& node) override;

    // Leaf nodes
    virtual void Visit(ErrorNode& node) override;
    virtual void Visit(IdentifierNode& node) override;
    virtual void Visit(IntegerLiteralNode& node) override;

    llvm::Value *GenerateCode(IntegerLiteralNode& node);

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
};

#endif // CODE_GENERATION_VISITOR_H_
