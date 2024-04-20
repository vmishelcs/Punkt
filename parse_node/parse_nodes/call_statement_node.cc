#include <llvm/IR/Value.h>

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include "call_statement_node.h"

void CallStatementNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *CallStatementNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
