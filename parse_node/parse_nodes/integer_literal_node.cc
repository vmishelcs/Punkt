#include <parse_node/parse_node_visitor.h>
#include <token/integer_literal_token.h>

#include "integer_literal_node.h"

IntegerLiteralNode::IntegerLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::INTEGER_LITERAL_NODE, std::move(token))
{}

int IntegerLiteralNode::GetValue() const {
    IntegerLiteralToken& integer_literal_token = dynamic_cast<IntegerLiteralToken&>(*(this->token));
    return integer_literal_token.GetValue();
}

std::string IntegerLiteralNode::AsString() const {
    return "INTEGER LITERAL NODE: " + token->AsString();
}

void IntegerLiteralNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}
