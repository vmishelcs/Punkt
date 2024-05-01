#ifndef EXPRESSION_STATEMENT_NODE_H_
#define EXPRESSION_STATEMENT_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>

#include <memory>
#include <string>

class ExpressionStatementNode : public ParseNode {
 public:
  ExpressionStatementNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::EXPRESSION_STATEMENT_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  ParseNode *GetExpressionNode() const { return GetChild(0); }

  virtual std::string ToString() const override {
    return "EXPRESSION STATEMENT NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // EXPRESSION_STATEMENT_NODE_H_
