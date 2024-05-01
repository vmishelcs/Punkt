#include "program_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>
#include <string>

std::unique_ptr<ParseNode> ProgramNode::CreateCopy() const {
  auto copy_node = std::make_unique<ProgramNode>(text_location);
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  copy_node->SetModuleID(module_id);
  return copy_node;
}

void ProgramNode::SetModuleID(const std::string module_id) {
  this->module_id = module_id;
}

void ProgramNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *ProgramNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
