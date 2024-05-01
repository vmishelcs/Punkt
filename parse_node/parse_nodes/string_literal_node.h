#ifndef STRING_LITERAL_NODE_H_
#define STRING_LITERAL_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

class StringLiteralNode : public ParseNode {
 public:
  StringLiteralNode(std::unique_ptr<Token> token);

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  std::string GetValue() const { return value; }

  virtual std::string ToString() const override {
    return "STRING LITERAL NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  std::string value;
};

#endif  // STRING_LITERAL_NODE_H_
