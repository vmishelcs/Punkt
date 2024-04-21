#ifndef INTEGER_LITERAL_NODE_H_
#define INTEGER_LITERAL_NODE_H_

#include <parse_node/parse_node.h>

class IntegerLiteralNode : public ParseNode {
 public:
  IntegerLiteralNode(std::unique_ptr<Token> token);

  int GetValue() const;

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor& visitor) override;

  virtual llvm::Value* GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif  // INTEGER_LITERAL_NODE_H_
