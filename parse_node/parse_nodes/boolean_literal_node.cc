#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/boolean_literal_token.h>

#include "boolean_literal_node.h"

BooleanLiteralNode::BooleanLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::BOOLEAN_LITERAL_NODE, std::move(token))
{}

bool BooleanLiteralNode::GetValue() const {
    BooleanLiteralToken& boolean_literal_token = dynamic_cast<BooleanLiteralToken&>(*(this->token));
    return boolean_literal_token.GetValue();
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
