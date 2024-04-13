#ifndef SEMANTIC_ANALYSIS_VISITOR_H_
#define SEMANTIC_ANALYSIS_VISITOR_H_

#include <parse_node/parse_node_visitor.h>

class SemanticAnalysisVisitor : public ParseNodeVisitor {
public:
    // ---- Non-leaf nodes -------------------------------------------------------------------
    virtual void VisitEnter(AssignmentStatementNode& node) override;
    virtual void VisitLeave(AssignmentStatementNode& node) override;

    virtual void VisitEnter(CodeBlockNode& node) override;
    virtual void VisitLeave(CodeBlockNode& node) override;

    virtual void VisitEnter(DeclarationStatementNode& node) override;
    virtual void VisitLeave(DeclarationStatementNode& node) override;

    virtual void VisitEnter(ForStatementNode& node) override;
    virtual void VisitLeave(ForStatementNode& node) override;

    virtual void VisitEnter(FunctionNode& node) override;
    virtual void VisitLeave(FunctionNode& node) override;

    virtual void VisitEnter(FunctionParameterNode& node) override;
    virtual void VisitLeave(FunctionParameterNode& node) override;

    virtual void VisitEnter(FunctionPrototypeNode& node) override;
    virtual void VisitLeave(FunctionPrototypeNode& node) override;

    virtual void VisitEnter(IfStatementNode& node) override;
    virtual void VisitLeave(IfStatementNode& node) override;

    virtual void VisitEnter(MainNode& node) override;
    virtual void VisitLeave(MainNode& node) override;

    virtual void VisitEnter(OperatorNode& node) override;
    virtual void VisitLeave(OperatorNode& node) override;

    virtual void VisitEnter(PrintStatementNode& node) override;
    virtual void VisitLeave(PrintStatementNode& node) override;

    virtual void VisitEnter(ProgramNode& node) override;
    virtual void VisitLeave(ProgramNode& node) override;

    virtual void VisitEnter(ReturnStatementNode& node) override;
    virtual void VisitLeave(ReturnStatementNode& node) override;

    // ---- Leaf nodes -----------------------------------------------------------------------
    virtual void Visit(ErrorNode& node) override;
    virtual void Visit(NopNode& node) override;
    virtual void Visit(IdentifierNode& node) override;
    virtual void Visit(BooleanLiteralNode& node) override;
    virtual void Visit(CharacterLiteralNode& node) override;
    virtual void Visit(IntegerLiteralNode& node) override;
    virtual void Visit(StringLiteralNode& node) override;
    virtual void Visit(TypeNode& node) override;

private:
    // ---- Miscellaneous helpers ------------------------------------------------------------
    void DeclareInLocalScope(IdentifierNode& node, bool is_mutable, Type *type);
    bool IsBeingDeclared(IdentifierNode& node);

    // ---- Scoping --------------------------------------------------------------------------
    void CreateGlobalScope(ProgramNode& node);
    void CreateSubscope(ParseNode& node);

    // ---- Error reporting ------------------------------------------------------------------
    void InvalidOperandTypeError(OperatorNode& node, std::vector<Type*>& types);
    void NonBooleanConditionError(IfStatementNode& node);
    void NonBooleanConditionError(ForStatementNode& node);
    void NonTargettableExpressionError(ParseNode& node);
    void AssignmentToImmutableTargetError(ParseNode& node);
    void AssignmentTypeMismatchError(ParseNode& node, const Type& target_type,
            const Type& value_type);
};

#endif // SEMANTIC_ANALYSIS_VISITOR_H_
