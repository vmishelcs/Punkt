#include "populated_array_expression_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>

std::unique_ptr<ParseNode> PopulatedArrayExpressionNode::CreateCopy() const {
  auto copy_node =
      std::make_unique<PopulatedArrayExpressionNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  return copy_node;
}

void PopulatedArrayExpressionNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *PopulatedArrayExpressionNode::GenerateCode(
    ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
