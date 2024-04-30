#include "program_node.h"

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

std::string ProgramNode::GetModuleID() const { return module_id; }

void ProgramNode::SetModuleID(const std::string module_id) {
  this->module_id = module_id;
}

std::string ProgramNode::ToString() const {
  return "PROGRAM NODE: " + token->ToString();
}

void ProgramNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *ProgramNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
