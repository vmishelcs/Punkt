#ifndef WHILE_STATEMENT_NODE_H_
#define WHILE_STATEMENT_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

class WhileStatementNode : public ParseNode {
 public:
  WhileStatementNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::WHILE_STATEMENT_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  ParseNode *GetConditionNode() const { return GetChild(0); }
  ParseNode *GetLoopBodyNode() const { return GetChild(1); }

  virtual std::string ToString() const { return "WHILE STATEMENT NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // WHILE_STATEMENT_NODE_H_
