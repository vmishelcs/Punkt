#include "if_statement_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

#include <memory>

std::unique_ptr<ParseNode> IfStatementNode::CreateCopy() const {
  auto copy_node = std::make_unique<IfStatementNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  return copy_node;
}

void IfStatementNode::AppendIf(std::unique_ptr<ParseNode> if_condition,
                               std::unique_ptr<ParseNode> if_block) {
  AppendChild(std::move(if_condition));
  AppendChild(std::move(if_block));
}

void IfStatementNode::AppendElse(std::unique_ptr<ParseNode> else_block) {
  AppendChild(std::move(else_block));
}

void IfStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *IfStatementNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
