#include "integer_literal_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

std::unique_ptr<ParseNode> IntegerLiteralNode::CreateCopy() const {
  return std::make_unique<IntegerLiteralNode>(token->CreateCopy());
}

void IntegerLiteralNode::Accept(ParseNodeVisitor &visitor) {
  visitor.Visit(*this);
}

llvm::Value *IntegerLiteralNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
