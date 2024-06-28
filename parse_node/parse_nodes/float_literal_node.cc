#include "float_literal_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

std::unique_ptr<ParseNode> FloatLiteralNode::CreateCopy() const {
  return std::make_unique<FloatLiteralNode>(token->CreateCopy());
}

void FloatLiteralNode::Accept(ParseNodeVisitor &visitor) {
  visitor.Visit(*this);
}

llvm::Value *FloatLiteralNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
