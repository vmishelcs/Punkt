#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/boolean_literal_token.h>

#include "boolean_literal_node.h"

BooleanLiteralNode::BooleanLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::BOOLEAN_LITERAL_NODE, std::move(token))
{
    BooleanLiteralToken *boolean_literal_token = (BooleanLiteralToken *)this->token.get();
    this->value = boolean_literal_token->GetValue();
}

BooleanLiteralNode::BooleanLiteralNode(bool value)
    : ParseNode(ParseNodeType::BOOLEAN_LITERAL_NODE, nullptr)
    , value(value)
{}

bool BooleanLiteralNode::GetValue() const {
    return value;
}

std::string BooleanLiteralNode::ToString() const {
    return "BOOLEAN LITERAL NODE: " + token->ToString();
}

void BooleanLiteralNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}

llvm::Value *BooleanLiteralNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
