#include "lambda_invocation_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <vector>

std::vector<ParseNode *> LambdaInvocationNode::GetArgumentNodes() const {
  std::vector<ParseNode *> result;
  if (NumChildren() == 1) {
    return result;
  }

  auto children = GetChildren();
  for (unsigned i = 1, n = NumChildren(); i < n; ++i) {
    result.push_back(children[i]);
  }
  return result;
}

void LambdaInvocationNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *LambdaInvocationNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
