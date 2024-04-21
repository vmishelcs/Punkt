#include "function_definition_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

FunctionDefinitionNode::FunctionDefinitionNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::FUNCTION_DEFINITION_NODE, std::move(token)) {}

void FunctionDefinitionNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *FunctionDefinitionNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
