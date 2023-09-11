#include <parse_node/parse_node_visitor.h>

#include "identifier_node.h"

IdentifierNode::IdentifierNode(std::unique_ptr<Token> token)
    : ParseNode(std::move(token))
{}

std::string IdentifierNode::GetNodeString() {
    return "IDENTIFIER NODE: " + token->GetTokenString();
}

void IdentifierNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}
