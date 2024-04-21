#include "assignment_statement_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

AssignmentStatementNode::AssignmentStatementNode()
    : ParseNode(ParseNodeType::ASSIGNMENT_STATEMENT_NODE, nullptr) {}

std::string AssignmentStatementNode::ToString() const {
  return "ASSIGNMENT STATEMENT NODE";
}

void AssignmentStatementNode::Accept(ParseNodeVisitor& visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value* AssignmentStatementNode::GenerateCode(
    ParseNodeIRVisitor& visitor) {
  return visitor.GenerateCode(*this);
}
