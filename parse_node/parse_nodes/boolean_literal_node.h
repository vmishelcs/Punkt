#ifndef BOOLEAN_LITERAL_NODE_H_
#define BOOLEAN_LITERAL_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

class BooleanLiteralNode : public ParseNode {
 public:
  BooleanLiteralNode(std::unique_ptr<Token> token);

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  bool GetValue() const { return value; }

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  bool value;
};

#endif  // BOOLEAN_LITERAL_NODE_H_
