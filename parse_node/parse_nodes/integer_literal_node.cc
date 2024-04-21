#include "integer_literal_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/integer_literal_token.h>

IntegerLiteralNode::IntegerLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::INTEGER_LITERAL_NODE, std::move(token)) {}

int IntegerLiteralNode::GetValue() const {
  IntegerLiteralToken &integer_literal_token =
      dynamic_cast<IntegerLiteralToken &>(*(this->token));
  return integer_literal_token.GetValue();
}

std::string IntegerLiteralNode::ToString() const {
  return "INTEGER LITERAL NODE: " + token->ToString();
}

void IntegerLiteralNode::Accept(ParseNodeVisitor &visitor) {
  visitor.Visit(*this);
}

llvm::Value *IntegerLiteralNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
