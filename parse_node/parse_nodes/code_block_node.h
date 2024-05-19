#ifndef CODE_BLOCK_NODE_H_
#define CODE_BLOCK_NODE_H_

#include <parse_node/parse_node.h>

#include <memory>
#include <string>

class CodeBlockNode : public ParseNode {
 public:
  CodeBlockNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::CODE_BLOCK_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  bool DoAllControlPathsReturn() const;

  virtual std::string ToString() const override { return "CodeBlockNode"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // CODE_BLOCK_NODE_H_
