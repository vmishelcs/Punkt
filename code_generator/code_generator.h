#ifndef CODE_GENERATOR_H_
#define CODE_GENERATOR_H_

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <parse_node/parse_node_visitor.h>

class CodeGenerator {
public:
    static void GenerateIR(std::unique_ptr<ParseNode> decorated_ast);

    // class CodeGenerationVisitor : public ParseNodeVisitor {
    // public:
    //     // Non-leaf nodes
    //     virtual void VisitEnter(CodeBlockNode& node) override;
    //     virtual void VisitLeave(CodeBlockNode& node) override;

    //     virtual void VisitEnter(DeclarationStatementNode& node) override;
    //     virtual void VisitLeave(DeclarationStatementNode& node) override;

    //     virtual void VisitEnter(MainNode& node) override;
    //     virtual void VisitLeave(MainNode& node) override;

    //     virtual void VisitEnter(OperatorNode& node) override;
    //     virtual void VisitLeave(OperatorNode& node) override;

    //     virtual void VisitEnter(PrintStatementNode& node) override;
    //     virtual void VisitLeave(PrintStatementNode& node) override;

    //     virtual void VisitEnter(ProgramNode& node) override;
    //     virtual void VisitLeave(ProgramNode& node) override;

    //     // Leaf nodes
    //     virtual void Visit(ErrorNode& node) override;
    //     virtual void Visit(IdentifierNode& node) override;
    //     virtual void Visit(IntegerLiteralNode& node) override;
    // };

private:
    CodeGenerator(std::unique_ptr<ParseNode> decorated_ast);

    // std::unique_ptr<ParseNode> decorated_ast;
    // std::unique_ptr<llvm::LLVMContext> context;
    // std::unique_ptr<llvm::Module> module;
    // std::unique_ptr<llvm::IRBuilder<>> builder;
};

#endif // CODE_GENERATOR_H_
