#ifndef ERROR_NODE_H_
#define ERROR_NODE_H_

#include <parse_node/parse_node.h>

#include <memory>
#include <string>

class ErrorNode : public ParseNode {
 public:
  ErrorNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::ERROR_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // ERROR_NODE_H_
