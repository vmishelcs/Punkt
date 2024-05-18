#ifndef DEALLOC_STATEMENT_NODE_H_
#define DEALLOC_STATEMENT_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

class DeallocStatementNode : public ParseNode {
 public:
  DeallocStatementNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::DEALLOC_STATEMENT_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  virtual std::string ToString() const override {
    return "DeallocStatementNode";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // DEALLOC_STATEMENT_NODE_H_
