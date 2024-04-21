#include "string_literal_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/string_literal_token.h>

StringLiteralNode::StringLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::STRING_LITERAL_NODE, std::move(token)) {}

std::string StringLiteralNode::GetValue() const {
  StringLiteralToken& string_literal_token =
      dynamic_cast<StringLiteralToken&>(*(this->token));
  return string_literal_token.GetValue();
}

std::string StringLiteralNode::ToString() const {
  return "STRING LITERAL NODE: " + token->ToString();
}

void StringLiteralNode::Accept(ParseNodeVisitor& visitor) {
  visitor.Visit(*this);
}

llvm::Value* StringLiteralNode::GenerateCode(ParseNodeIRVisitor& visitor) {
  return visitor.GenerateCode(*this);
}
