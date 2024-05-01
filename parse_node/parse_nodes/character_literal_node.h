#ifndef CHARACTER_LITERAL_NODE_H_
#define CHARACTER_LITERAL_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>

#include <memory>
#include <string>

class CharacterLiteralNode : public ParseNode {
 public:
  CharacterLiteralNode(std::unique_ptr<Token> token);

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  char GetValue() const { return value; }

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  char value;
};

#endif  // CHARACTER_LITERAL_NODE_H_
