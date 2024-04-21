#include "error_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

ErrorNode::ErrorNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::ERROR_NODE, std::move(token)) {}

std::string ErrorNode::ToString() const {
  return "SYNTAX ERROR: " + token->ToString();
}

void ErrorNode::Accept(ParseNodeVisitor& visitor) { visitor.Visit(*this); }

llvm::Value* ErrorNode::GenerateCode(ParseNodeIRVisitor& visitor) {
  return visitor.GenerateCode(*this);
}
