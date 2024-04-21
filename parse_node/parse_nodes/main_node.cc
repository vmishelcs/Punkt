#include "main_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

MainNode::MainNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::MAIN_NODE, std::move(token)) {}

std::string MainNode::ToString() const {
  return "MAIN NODE: " + token->ToString();
}

void MainNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *MainNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
