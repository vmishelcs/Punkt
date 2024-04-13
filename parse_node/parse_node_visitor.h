#ifndef PARSE_NODE_VISITOR_H_
#define PARSE_NODE_VISITOR_H_

#include "parse_nodes/all_nodes.h"

class ParseNodeVisitor {
public:
    // ---- Non-leaf nodes -------------------------------------------------------------------
    virtual void VisitEnter(AssignmentStatementNode& node) = 0;
    virtual void VisitLeave(AssignmentStatementNode& node) = 0;

    virtual void VisitEnter(CodeBlockNode& node) = 0;
    virtual void VisitLeave(CodeBlockNode& node) = 0;

    virtual void VisitEnter(DeclarationStatementNode& node) = 0;
    virtual void VisitLeave(DeclarationStatementNode& node) = 0;

    virtual void VisitEnter(ForStatementNode& node) = 0;
    virtual void VisitLeave(ForStatementNode& node) = 0;

    virtual void VisitEnter(FunctionNode& node) = 0;
    virtual void VisitLeave(FunctionNode& node) = 0;

    virtual void VisitEnter(FunctionParameterNode& node) = 0;
    virtual void VisitLeave(FunctionParameterNode& node) = 0;

    virtual void VisitEnter(FunctionPrototypeNode& node) = 0;
    virtual void VisitLeave(FunctionPrototypeNode& node) = 0;

    virtual void VisitEnter(IfStatementNode& node) = 0;
    virtual void VisitLeave(IfStatementNode& node) = 0;

    virtual void VisitEnter(MainNode& node) = 0;
    virtual void VisitLeave(MainNode& node) = 0;

    virtual void VisitEnter(OperatorNode& node) = 0;
    virtual void VisitLeave(OperatorNode& node) = 0;

    virtual void VisitEnter(PrintStatementNode& node) = 0;
    virtual void VisitLeave(PrintStatementNode& node) = 0;

    virtual void VisitEnter(ProgramNode& node) = 0;
    virtual void VisitLeave(ProgramNode& node) = 0;

    virtual void VisitEnter(ReturnStatementNode& node) = 0;
    virtual void VisitLeave(ReturnStatementNode& node) = 0;

    // ---- Leaf nodes -----------------------------------------------------------------------
    virtual void Visit(ErrorNode& node) = 0;
    virtual void Visit(NopNode& node) = 0;
    virtual void Visit(IdentifierNode& node) = 0;
    virtual void Visit(BooleanLiteralNode& node) = 0;
    virtual void Visit(CharacterLiteralNode& node) = 0;
    virtual void Visit(IntegerLiteralNode& node) = 0;
    virtual void Visit(StringLiteralNode& node) = 0;
    virtual void Visit(TypeNode& node) = 0;

protected:
    int depth;
};

#endif // PARSE_NODE_VISITOR_H_
