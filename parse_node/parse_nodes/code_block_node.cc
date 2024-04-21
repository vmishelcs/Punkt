#include "code_block_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

CodeBlockNode::CodeBlockNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::CODE_BLOCK_NODE, std::move(token)) {}

std::string CodeBlockNode::ToString() const {
  return "CODE BLOCK NODE: " + token->ToString();
}

void CodeBlockNode::Accept(ParseNodeVisitor& visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value* CodeBlockNode::GenerateCode(ParseNodeIRVisitor& visitor) {
  return visitor.GenerateCode(*this);
}
