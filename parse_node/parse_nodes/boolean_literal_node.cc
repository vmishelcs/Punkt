#include "boolean_literal_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/boolean_literal_token.h>

#include <memory>

BooleanLiteralNode::BooleanLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::BOOLEAN_LITERAL_NODE, std::move(token)) {
  auto bool_literal_token =
      static_cast<BooleanLiteralToken *>(this->token.get());
  this->value = bool_literal_token->GetValue();
}

std::unique_ptr<ParseNode> BooleanLiteralNode::CreateCopy() const {
  return std::make_unique<BooleanLiteralNode>(token->CreateCopy());
}

std::string BooleanLiteralNode::ToString() const {
  return "BOOLEAN LITERAL NODE: " + token->ToString();
}

void BooleanLiteralNode::Accept(ParseNodeVisitor &visitor) {
  visitor.Visit(*this);
}

llvm::Value *BooleanLiteralNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
