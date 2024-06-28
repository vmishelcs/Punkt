#ifndef FLOAT_LITERAL_NODE_H_
#define FLOAT_LITERAL_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/float_literal_token.h>

#include <memory>
#include <string>

class FloatLiteralNode : public ParseNode {
 public:
  FloatLiteralNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::FLOAT_LITERAL_NODE, std::move(token)) {
    auto int_literal_token =
        static_cast<FloatLiteralToken *>(this->token.get());
    this->value = int_literal_token->GetValue();
  }

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  double GetValue() const { return value; }

  virtual std::string ToString() const override { return "FLOAT LITERAL NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  double value;
};

#endif  // FLOAT_LITERAL_NODE_H_
