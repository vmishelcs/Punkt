#ifndef SEMANTIC_ANALYSIS_VISITOR_H_
#define SEMANTIC_ANALYSIS_VISITOR_H_

#include <parse_node/parse_node_visitor.h>

class SemanticAnalysisVisitor : public ParseNodeVisitor {
public:
    // Non-leaf nodes
    virtual void VisitEnter(CodeBlockNode& node);
    virtual void VisitLeave(CodeBlockNode& node);

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
    std::unique_ptr<ParseNode> ast;
};

#endif // SEMANTIC_ANALYSIS_VISITOR_H_
