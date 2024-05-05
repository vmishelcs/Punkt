#include "error_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>

std::unique_ptr<ParseNode> ErrorNode::CreateCopy() const {
  return std::make_unique<ErrorNode>(token->CreateCopy());
}

std::string ErrorNode::ToString() const {
  return "SYNTAX ERROR: " + token->ToString();
}

void ErrorNode::Accept(ParseNodeVisitor &visitor) { visitor.Visit(*this); }

llvm::Value *ErrorNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
