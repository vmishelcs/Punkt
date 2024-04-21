#include "return_statement_node.h"

#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <string>

#include "lambda_node.h"

ParseNode* ReturnStatementNode::GetEnclosingFunctionNode() {
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

  return enclosing_function_node;
}

std::string ReturnStatementNode::ToString() const {
  return "RETURN STATEMENT NODE: " + token->ToString();
}

void ReturnStatementNode::Accept(ParseNodeVisitor& visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value* ReturnStatementNode::GenerateCode(ParseNodeIRVisitor& visitor) {
  return visitor.GenerateCode(*this);
}
