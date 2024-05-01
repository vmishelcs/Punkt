#include "return_statement_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>
#include <string>

#include "lambda_node.h"

std::unique_ptr<ParseNode> ReturnStatementNode::CreateCopy() const {
  auto copy_node = std::make_unique<ReturnStatementNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  copy_node->enclosing_function_node = this->enclosing_function_node;
  return copy_node;
}

ParseNode *ReturnStatementNode::GetEnclosingFunctionNode() {
  if (enclosing_function_node) {
    return enclosing_function_node;
  }

  enclosing_function_node = GetParent();
  while (enclosing_function_node) {
    if (enclosing_function_node->GetParseNodeType() ==
        ParseNodeType::LAMBDA_NODE) {
      return enclosing_function_node;
    }
    if (enclosing_function_node->GetParseNodeType() ==
        ParseNodeType::MAIN_NODE) {
      return enclosing_function_node;
    }

    enclosing_function_node = enclosing_function_node->GetParent();
  }

  return nullptr;
}

void ReturnStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *ReturnStatementNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
