#ifndef STRING_LITERAL_NODE_H_
#define STRING_LITERAL_NODE_H_

#include <parse_node/parse_node.h>

class StringLiteralNode : public ParseNode {
 public:
  StringLiteralNode(std::unique_ptr<Token> token);

  std::string GetValue() const;

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor& visitor) override;

  llvm::Value* GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif  // STRING_LITERAL_NODE_H_
