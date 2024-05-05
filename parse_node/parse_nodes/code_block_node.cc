#include "code_block_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>

std::unique_ptr<ParseNode> CodeBlockNode::CreateCopy() const {
  auto copy_node = std::make_unique<CodeBlockNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  return copy_node;
}

std::string CodeBlockNode::ToString() const {
  return "CODE BLOCK NODE: " + token->ToString();
}

void CodeBlockNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *CodeBlockNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
