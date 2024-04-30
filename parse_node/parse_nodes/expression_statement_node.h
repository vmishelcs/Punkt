#ifndef EXPRESSION_STATEMENT_NODE_H_
#define EXPRESSION_STATEMENT_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>

class ExpressionStatementNode : public ParseNode {
 public:
  ExpressionStatementNode(TextLocation text_location)
      : ParseNode(ParseNodeType::EXPRESSION_STATEMENT_NODE, text_location) {}

  ParseNode *GetExpressionNode() const { return GetChild(0); }

  virtual std::string ToString() const override {
    return "EXPRESSION STATEMENT NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // EXPRESSION_STATEMENT_NODE_H_
