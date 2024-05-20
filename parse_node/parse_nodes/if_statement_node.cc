#include "if_statement_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

#include <memory>

std::unique_ptr<ParseNode> IfStatementNode::CreateCopy() const {
  auto copy_node = std::make_unique<IfStatementNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  return copy_node;
}

void IfStatementNode::AppendIf(std::unique_ptr<ParseNode> if_condition,
                               std::unique_ptr<ParseNode> if_block) {
  AppendChild(std::move(if_condition));
  AppendChild(std::move(if_block));
}

void IfStatementNode::AppendElse(std::unique_ptr<ParseNode> else_block) {
  AppendChild(std::move(else_block));
}

bool IfStatementNode::DoAllControlPathsReturn() const {
  // If this method is called, we must make sure all control paths of this
  // if-statement return.

  // Make sure the 'if' path returns.
  bool if_path_returns = false;
  ParseNode *if_path = GetIfBlockNode();
  if (auto code_block = dynamic_cast<CodeBlockNode *>(if_path)) {
    // If the 'if' path is a code-block, make sure it returns eventually.
    if_path_returns = code_block->DoAllControlPathsReturn();
  } else if (auto if_stmt = dynamic_cast<IfStatementNode *>(if_path)) {
    // If the 'if' path is an if-statement, make sure it returns.
    if_path_returns = if_stmt->DoAllControlPathsReturn();
  } else {
    // If the 'if' path is neither an if-statement nor a code-block, then it
    // must be a single statement. Check that this single statement is a return
    // statement.
    if_path_returns = dynamic_cast<ReturnStatementNode *>(if_path) != nullptr;
  }

  // If the 'else' path does not exist, return the result from analyzing the
  // 'if' path.
  if (!HasElseBlock()) {
    return if_path_returns;
  }

  bool else_path_returns = false;
  ParseNode *else_path = GetElseBlockNode();
  if (auto else_code_block = dynamic_cast<CodeBlockNode *>(else_path)) {
    // If the 'else' path is a code-block, make sure it returns eventually.
    else_path_returns = else_code_block->DoAllControlPathsReturn();
  } else if (auto if_stmt = dynamic_cast<IfStatementNode *>(else_path)) {
    // If the 'else' path is an if-statement, make sure it returns.
    else_path_returns = if_stmt->DoAllControlPathsReturn();
  } else {
    // If the 'else' path is neither an if-statement nor a code-block, then it
    // must be a single statement. Check that this single statement is a return
    // statement.
    else_path_returns =
        dynamic_cast<ReturnStatementNode *>(else_path) != nullptr;
  }

  return if_path_returns && else_path_returns;
}

void IfStatementNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *IfStatementNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
