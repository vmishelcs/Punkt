#include "nop_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

std::unique_ptr<ParseNode> NopNode::CreateCopy() const {
  return std::make_unique<NopNode>(token->CreateCopy());
}

void NopNode::Accept(ParseNodeVisitor &visitor) { visitor.Visit(*this); }

llvm::Value *NopNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
