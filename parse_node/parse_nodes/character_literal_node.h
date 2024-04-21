#ifndef CHARACTER_LITERAL_NODE_H_
#define CHARACTER_LITERAL_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>

class CharacterLiteralNode : public ParseNode {
 public:
  CharacterLiteralNode(std::unique_ptr<Token> token);

  char GetValue() const;

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // CHARACTER_LITERAL_NODE_H_
