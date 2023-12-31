#include <parse_node/parse_node_visitor.h>

#include "declaration_statement_node.h"

DeclarationStatementNode::DeclarationStatementNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::DECLARATION_STATEMENT_NODE, std::move(token))
{}

std::string DeclarationStatementNode::AsString() const {
    return "DECLARATION STATEMENT NODE: " + token->AsString();
}

void DeclarationStatementNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}
