#ifndef SEMANTIC_ANALYSIS_VISITOR_H_
#define SEMANTIC_ANALYSIS_VISITOR_H_

#include <parse_node/parse_node_visitor.h>

class SemanticAnalysisVisitor : public ParseNodeVisitor {
public:
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

private:
    // Miscellaneous helpers
    void Declare(IdentifierNode& node, bool is_mutable, const Type& type);
    bool IsBeingDeclared(IdentifierNode& node);

    // Scoping
    void CreateGlobalScope(ProgramNode& node);
    void CreateSubscope(CodeBlockNode& node);

    // Error reporting
    void InvalidOperandTypeError(OperatorNode& node, std::vector<std::reference_wrapper<const Type>>& types);
};

#endif // SEMANTIC_ANALYSIS_VISITOR_H_
