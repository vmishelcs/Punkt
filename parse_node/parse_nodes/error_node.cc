#include <parse_node/parse_node_visitor.h>

#include "error_node.h"

ErrorNode::ErrorNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::ERROR_NODE, std::move(token))
{}

std::string ErrorNode::AsString() const {
    return "SYNTAX ERROR: " + token->AsString();
}

void ErrorNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}
