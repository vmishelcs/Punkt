#include "array_type_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <semantic_analyzer/types/array_type.h>
#include <semantic_analyzer/types/base_type.h>

std::unique_ptr<ParseNode> ArrayTypeNode::CreateCopy() const {
  auto copy_node = std::make_unique<ArrayTypeNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }

  return copy_node;
}

std::unique_ptr<Type> ArrayTypeNode::InferOwnType() const {
  Type *subtype = GetChild(0)->GetType();
  if (subtype->IsErrorType()) {
    return BaseType::CreateErrorType();
  }
  return ArrayType::CreateArrayType(subtype);
}

void ArrayTypeNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *ArrayTypeNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
