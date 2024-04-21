#include "expression_statement_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

void ExpressionStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *ExpressionStatementNode::GenerateCode(
    ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
