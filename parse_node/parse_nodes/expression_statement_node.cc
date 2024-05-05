#include "expression_statement_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>

std::unique_ptr<ParseNode> ExpressionStatementNode::CreateCopy() const {
  auto copy_node =
      std::make_unique<ExpressionStatementNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  return copy_node;
}

void ExpressionStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *ExpressionStatementNode::GenerateCode(
    ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
