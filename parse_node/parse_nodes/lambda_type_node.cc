#include "lambda_type_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <semantic_analyzer/types/lambda_type.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

std::unique_ptr<ParseNode> LambdaTypeNode::CreateCopy() const {
  auto copy_node = std::make_unique<LambdaTypeNode>(token->CreateCopy());

  // Copy parameter type nodes.
  for (auto param : GetParameterTypeNodes()) {
    copy_node->AddParameterTypeNode(param->CreateCopy());
  }

  // Copy return type node.
  copy_node->AddReturnTypeNode(return_type_node->CreateCopy());

  return copy_node;
}

std::unique_ptr<Type> LambdaTypeNode::InferOwnType() const {
  std::vector<Type *> parameter_types;
  parameter_types.reserve(parameter_type_nodes.size());

  // Fill parameter types vector with corresponding types.
  std::transform(parameter_type_nodes.begin(), parameter_type_nodes.end(),
                 std::inserter(parameter_types, parameter_types.end()),
                 [](const auto &param_node) { return param_node->GetType(); });

  return LambdaType::CreateLambdaType(parameter_types,
                                      return_type_node->GetType());
}

void LambdaTypeNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *LambdaTypeNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
