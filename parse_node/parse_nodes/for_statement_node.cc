#include "for_statement_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

#include <memory>

std::unique_ptr<ParseNode> ForStatementNode::CreateCopy() const {
  auto copy_node = std::make_unique<ForStatementNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  return copy_node;
}

std::string ForStatementNode::ToString() const {
  return "FOR STATEMENT NODE: " + token->ToString();
}

void ForStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *ForStatementNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
