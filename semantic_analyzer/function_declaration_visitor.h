#ifndef FUNCTION_DECLARATION_VISITOR_H_
#define FUNCTION_DECLARATION_VISITOR_H_

#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_nodes/all_nodes.h>

class FunctionDeclarationVisitor : public DefaultParseNodeVisitor {
public:
    // ---- Non-leaf nodes -------------------------------------------------------------------
    virtual void VisitEnter(FunctionNode& node) override;
    virtual void VisitLeave(FunctionNode& node) override;

    virtual void VisitLeave(FunctionParameterNode& node) override;

    virtual void VisitEnter(FunctionPrototypeNode& node) override;
    virtual void VisitLeave(FunctionPrototypeNode& node) override;

    virtual void VisitEnter(ProgramNode& node) override;

    // ---- Leaf nodes -----------------------------------------------------------------------
    virtual void Visit(TypeNode& node) override;

private:
    // ---- Scoping --------------------------------------------------------------------------
    void CreateGlobalScope(ParseNode& node);
};

#endif // FUNCTION_DECLARATION_VISITOR_H_
