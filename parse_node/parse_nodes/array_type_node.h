#ifndef ARRAY_TYPE_NODE_H_
#define ARRAY_TYPE_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

class ArrayTypeNode : public ParseNode {
 public:
  ArrayTypeNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::ARRAY_TYPE_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  /// @brief Get the type of this `ArrayTypeNode`.
  /// @return A `unique_ptr<Type>` object representing the type of this
  /// node.
  std::unique_ptr<Type> InferOwnType() const;

  virtual std::string ToString() const override { return "ARRAY TYPE NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // ARRAY_TYPE_NODE_H_
