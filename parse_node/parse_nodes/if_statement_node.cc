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

bool IfStatementNode::HasElseBlock() const {
  // If-statement has an 'else' block if there are more than two children
  // (first 2 children are the condition and the 'then' block).
  return NumChildren() > 2;
}

void IfStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *IfStatementNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
