#ifndef INTEGER_LITERAL_NODE_H_
#define INTEGER_LITERAL_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/integer_literal_token.h>

#include <cstdint>
#include <memory>
#include <string>

class IntegerLiteralNode : public ParseNode {
 public:
  IntegerLiteralNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::INTEGER_LITERAL_NODE, std::move(token)) {
    auto int_literal_token =
        static_cast<IntegerLiteralToken *>(this->token.get());
    this->value = int_literal_token->GetValue();
  }

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  int64_t GetValue() const { return value; }

  virtual std::string ToString() const override {
    return "INTEGER LITERAL NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  int64_t value;
};

#endif  // INTEGER_LITERAL_NODE_H_
