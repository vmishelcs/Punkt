#ifndef BASE_TYPE_NODE_H_
#define BASE_TYPE_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <semantic_analyzer/types/type.h>
#include <token/token.h>

#include <memory>
#include <string>

class BaseTypeNode : public ParseNode {
 public:
  BaseTypeNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::LAMBDA_TYPE_NODE, std::move(token)) {}

  /// @brief Get the type of this `BaseTypeNode`.
  /// @return A `unique_ptr<BaseType>` object representing the type of this
  /// node.
  std::unique_ptr<Type> InferOwnType() const;

  virtual std::string ToString() const override { return "BASE TYPE NODE"; }

  virtual void Accept(ParseNodeVisitor& visitor) override;

  virtual llvm::Value* GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif  // BASE_TYPE_NODE_H_
