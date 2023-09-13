#include <parse_node/parse_node_visitor.h>

#include "integer_literal_node.h"

IntegerLiteralNode::IntegerLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::INTEGER_LITERAL_NODE, std::move(token))
{}

std::string IntegerLiteralNode::GetNodeString() {
    return "INTEGER LITERAL NODE: " + token->GetTokenString();
}

void IntegerLiteralNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}
