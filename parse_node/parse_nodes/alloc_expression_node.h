#ifndef ALLOC_EXPRESSION_NODE_H_
#define ALLOC_EXPRESSION_NODE_H_

#include <parse_node/parse_node.h>

#include <memory>
#include <string>

class AllocExpressionNode : public ParseNode {
 public:
  AllocExpressionNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::ALLOC_EXPRESSION_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  virtual std::string ToString() const override {
    return "AllocExpressionNode";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // ALLOC_EXPRESSION_NODE_H_
