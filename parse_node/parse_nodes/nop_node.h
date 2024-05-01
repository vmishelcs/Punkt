#ifndef NOP_NODE_H_
#define NOP_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>

#include <memory>
#include <string>

class NopNode : public ParseNode {
 public:
  NopNode(TextLocation text_location)
      : ParseNode(ParseNodeType::FOR_STATEMENT_NODE, text_location) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  virtual std::string ToString() const override { return "NOP NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // NOP_NODE_H_
