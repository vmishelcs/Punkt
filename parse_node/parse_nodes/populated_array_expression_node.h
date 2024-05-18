#ifndef POPULATED_ARRAY_EXPRESSION_NODE_H_
#define POPULATED_ARRAY_EXPRESSION_NODE_H_

#include <parse_node/parse_node.h>

#include <memory>
#include <string>

class PopulatedArrayExpressionNode : public ParseNode {
 public:
  PopulatedArrayExpressionNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::POPULATED_ARRAY_EXPRESSION_NODE,
                  std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  virtual std::string ToString() const override {
    return "PopulatedArrayExpressionNode";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // POPULATED_ARRAY_EXPRESSION_NODE_H_
