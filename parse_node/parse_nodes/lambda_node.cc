#include "lambda_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>
#include <string>
#include <vector>

#include "declaration_statement_node.h"
#include "function_definition_node.h"
#include "lambda_parameter_node.h"
#include "operator_node.h"

std::unique_ptr<ParseNode> LambdaNode::CreateCopy() const {
  auto copy_node = std::make_unique<LambdaNode>(token->CreateCopy());

  // Copy over the parameter nodes.
  for (auto param : GetParameterNodes()) {
    copy_node->AddParameterNode(param->CreateCopy());
  }

  // Copy over return type node.
  copy_node->AddReturnTypeNode(return_type_node->CreateCopy());

  // Copy over the lambda body.
  copy_node->AddLambdaBodyNode(lambda_body->CreateCopy());

  return copy_node;
}

void LambdaNode::AddParameterNode(std::unique_ptr<ParseNode> parameter_node) {
  this->parameter_nodes.push_back(
      dynamic_cast<LambdaParameterNode *>(parameter_node.get()));
  this->AppendChild(std::move(parameter_node));
}

void LambdaNode::AddReturnTypeNode(
    std::unique_ptr<ParseNode> return_type_node) {
  this->return_type_node = return_type_node.get();
  this->AppendChild(std::move(return_type_node));
}

void LambdaNode::AddLambdaBodyNode(std::unique_ptr<ParseNode> lambda_body) {
  this->lambda_body = lambda_body.get();
  this->AppendChild(std::move(lambda_body));
}

bool LambdaNode::IsAnonymous() const {
  if (dynamic_cast<FunctionDefinitionNode *>(GetParent())) {
    return false;
  }
  if (dynamic_cast<DeclarationStatementNode *>(GetParent())) {
    return false;
  }
  if (auto parent = dynamic_cast<OperatorNode *>(GetParent())) {
    return parent->GetOperatorEnum() != Operator::ASSIGN;
  }

  return true;
}

void LambdaNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *LambdaNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
