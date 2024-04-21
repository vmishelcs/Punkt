#include "nop_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

NopNode::NopNode() : ParseNode(ParseNodeType::FOR_STATEMENT_NODE, nullptr) {}

std::string NopNode::ToString() const { return "NOP NODE"; }

void NopNode::Accept(ParseNodeVisitor &visitor) { visitor.Visit(*this); }

llvm::Value *NopNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
