#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>

#include "declaration_statement_node.h"

DeclarationStatementNode::DeclarationStatementNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::DECLARATION_STATEMENT_NODE, std::move(token))
{}

std::string DeclarationStatementNode::ToString() const {
    return "DECLARATION STATEMENT NODE: " + token->ToString();
}

void DeclarationStatementNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *DeclarationStatementNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
