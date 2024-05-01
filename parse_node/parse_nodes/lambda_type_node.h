#ifndef LAMBDA_TYPE_NODE_H_
#define LAMBDA_TYPE_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <semantic_analyzer/types/type.h>
#include <token/token.h>

#include <memory>
#include <string>
#include <vector>

class LambdaTypeNode : public ParseNode {
 public:
  LambdaTypeNode(TextLocation text_location)
      : ParseNode(ParseNodeType::LAMBDA_TYPE_NODE, text_location),
        parameter_type_nodes(),
        return_type_node{nullptr} {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  /// @brief Attach the specified `ParseNode` to `this` node to represent a
  /// parameter type.
  /// @param parameter_node A `ParseNode` representing a lambda parameter type.
  /// @warning This method attaches its argument using
  /// `ParseNode::AppendChild`, appending it to the end of the children vector.
  void AddParameterTypeNode(std::unique_ptr<ParseNode> parameter_type_node) {
    parameter_type_nodes.push_back(parameter_type_node.get());
    AppendChild(std::move(parameter_type_node));
  }

  /// @brief Attach the specified `ParseNode` to `this` node to represent a
  /// return type.
  /// @param parameter_node A `ParseNode` representing a lambda return type.
  /// @warning This method attaches its argument using
  /// `ParseNode::AppendChild`, appending it to the end of the children vector.
  void AddReturnTypeNode(std::unique_ptr<ParseNode> return_type_node) {
    this->return_type_node = return_type_node.get();
    AppendChild(std::move(return_type_node));
  }

  std::vector<ParseNode *> GetParameterTypeNodes() const {
    return parameter_type_nodes;
  }
  ParseNode *GetReturnTypeNode() const { return return_type_node; }

  /// @brief Get the type of this `LambdaTypeNode`.
  /// @return A `unique_ptr<Type>` object representing the type of this node.
  std::unique_ptr<Type> InferOwnType() const;

  virtual std::string ToString() const override { return "LAMBDA TYPE NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  std::vector<ParseNode *> parameter_type_nodes;
  ParseNode *return_type_node;
};

#endif  // LAMBDA_TYPE_NODE_H_
