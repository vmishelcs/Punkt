#include <parse_node/parse_node_visitor.h>

#include "error_node.h"

ErrorNode::ErrorNode(std::unique_ptr<Token> token)
    : ParseNode(std::move(token))
{}

std::string ErrorNode::GetNodeString() {
    return "SYNTAX ERROR: " + token->GetTokenString();
}

void ErrorNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}
