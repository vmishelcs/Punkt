#include "for_statement_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

ForStatementNode::ForStatementNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::FOR_STATEMENT_NODE, std::move(token)) {}

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
