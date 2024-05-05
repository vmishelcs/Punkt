#include "string_literal_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/string_literal_token.h>

StringLiteralNode::StringLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::STRING_LITERAL_NODE, std::move(token)) {
  auto string_literal_token =
      static_cast<StringLiteralToken *>(this->token.get());
  this->value = string_literal_token->GetValue();
}

std::unique_ptr<ParseNode> StringLiteralNode::CreateCopy() const {
  return std::make_unique<StringLiteralNode>(token->CreateCopy());
}

void StringLiteralNode::Accept(ParseNodeVisitor &visitor) {
  visitor.Visit(*this);
}

llvm::Value *StringLiteralNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
