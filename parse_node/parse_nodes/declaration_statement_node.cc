#include "declaration_statement_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>

std::unique_ptr<ParseNode> DeclarationStatementNode::CreateCopy() const {
  auto copy_node =
      std::make_unique<DeclarationStatementNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  return copy_node;
}

std::string DeclarationStatementNode::ToString() const {
  return "DECLARATION STATEMENT NODE: " + token->ToString();
}

void DeclarationStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *DeclarationStatementNode::GenerateCode(
    ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
