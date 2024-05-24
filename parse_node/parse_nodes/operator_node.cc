#include "operator_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/operator_token.h>

#include <cassert>
#include <memory>

OperatorNode::OperatorNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::OPERATOR_NODE, std::move(token)) {
  auto op_token = static_cast<OperatorToken *>(this->token.get());
  this->op = op_token->GetOperatorEnum();
}

std::unique_ptr<ParseNode> OperatorNode::CreateCopy() const {
  auto copy_node = std::make_unique<OperatorNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }

  copy_node->op = this->op;

  return copy_node;
}

bool OperatorNode::IsAssignmentTarget() const {
  ParseNode *parent = GetParent();
  auto op_node = dynamic_cast<OperatorNode *>(parent);
  if (op_node && op_node->GetOperatorEnum() == Operator::ASSIGN &&
      op_node->GetChild(0) == this) {
    return true;
  }
  return false;
}

bool OperatorNode::IsArithmeticOperation() const {
  return op == Operator::PLUS || op == Operator::MINUS || op == Operator::DIV ||
         op == Operator::MUL;
}

void OperatorNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *OperatorNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
