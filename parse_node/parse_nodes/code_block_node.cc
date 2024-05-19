#include "code_block_node.h"

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include <memory>

#include "if_statement_node.h"
#include "return_statement_node.h"

std::unique_ptr<ParseNode> CodeBlockNode::CreateCopy() const {
  auto copy_node = std::make_unique<CodeBlockNode>(token->CreateCopy());
  for (auto child : GetChildren()) {
    copy_node->AppendChild(child->CreateCopy());
  }
  return copy_node;
}

bool CodeBlockNode::DoAllControlPathsReturn() const {
  unsigned num_stmts = NumChildren();

  if (num_stmts == 0) {
    // If there are no statements in this code-block, return false.
    return false;
  }

  // Check each statement.
  for (unsigned i = 0; i < num_stmts; ++i) {
    ParseNode *stmt = this->GetChild(i);

    auto return_stmt = dynamic_cast<ReturnStatementNode *>(stmt);
    if (return_stmt) {
      // This code-block definitely returns.
      return true;
    }

    auto code_block = dynamic_cast<CodeBlockNode *>(stmt);
    if (code_block && i == num_stmts - 1) {
      // If the last statement is a code-block, make sure all control paths
      // return of this code-block.
      return code_block->DoAllControlPathsReturn();
    }

    auto if_stmt = dynamic_cast<IfStatementNode *>(stmt);
    if (if_stmt && i == num_stmts - 1) {
      // If this if statement is the last statement in this code-block, then we
      // must make sure that all control paths of this if statement return.
      return if_stmt->DoAllControlPathsReturn();
    }
  }

  return false;
}

void CodeBlockNode::Accept(ParseNodeVisitor &visitor) {
  visitor.VisitEnter(*this);
  VisitChildren(visitor);
  visitor.VisitLeave(*this);
}

llvm::Value *CodeBlockNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
