#ifndef MAIN_NODE_H_
#define MAIN_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

class MainNode : public ParseNode {
 public:
  MainNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::MAIN_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  virtual std::string ToString() const override { return "MAIN NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // MAIN_NODE_H_
