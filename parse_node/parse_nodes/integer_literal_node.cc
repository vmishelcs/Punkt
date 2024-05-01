#include "integer_literal_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/integer_literal_token.h>

IntegerLiteralNode::IntegerLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::INTEGER_LITERAL_NODE, std::move(token)) {
  auto int_literal_token =
      static_cast<IntegerLiteralToken *>(this->token.get());
  this->value = int_literal_token->GetValue();
}

std::unique_ptr<ParseNode> IntegerLiteralNode::CreateCopy() const {
  return std::make_unique<IntegerLiteralNode>(token->CreateCopy());
}

void IntegerLiteralNode::Accept(ParseNodeVisitor &visitor) {
  visitor.Visit(*this);
}

llvm::Value *IntegerLiteralNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
