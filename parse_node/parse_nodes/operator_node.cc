#include "operator_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/punctuator_token.h>

#include <memory>

OperatorNode::OperatorNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::OPERATOR_NODE, std::move(token)) {
  auto punctuator_token = static_cast<PunctuatorToken *>(this->token.get());
  punctuator = punctuator_token->GetPunctuatorEnum();
}

std::unique_ptr<ParseNode> OperatorNode::CreateCopy() const {
  auto copy_node = std::make_unique<OperatorNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }

  copy_node->punctuator = this->punctuator;

  return copy_node;
}

void OperatorNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *OperatorNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
