#ifndef INTEGER_LITERAL_NODE_H_
#define INTEGER_LITERAL_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

class IntegerLiteralNode : public ParseNode {
 public:
  IntegerLiteralNode(std::unique_ptr<Token> token);

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  int GetValue() const { return value; }

  virtual std::string ToString() const override {
    return "INTEGER LITERAL NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  int value;
};

#endif  // INTEGER_LITERAL_NODE_H_
