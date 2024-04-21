#include "print_statement_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

PrintStatementNode::PrintStatementNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::PRINT_STATEMENT_NODE, std::move(token)) {}

std::string PrintStatementNode::ToString() const {
  return "PRINT STATEMENT NODE: " + token->ToString();
}

void PrintStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *PrintStatementNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
