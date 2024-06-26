#ifndef LAMBDA_NODE_H_
#define LAMBDA_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <scanner/operator.h>
#include <token/token.h>

#include <memory>
#include <string>
#include <vector>

#include "code_block_node.h"
#include "lambda_parameter_node.h"

class LambdaNode : public ParseNode {
 public:
  LambdaNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::LAMBDA_NODE, std::move(token)),
        parameter_nodes(),
        return_type_node{nullptr},
        lambda_body{nullptr} {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  /// @brief Attach the specified `ParseNode` to `this` node to represent a
  /// parameter.
  /// @param parameter_node A `ParseNode` representing a lambda parameter.
  /// @warning This method attaches its argument using
  /// `ParseNode::AppendChild`, appending it to the end of the children vector
  /// field.
  void AddParameterNode(std::unique_ptr<ParseNode> parameter_node);

  /// @brief Attach the specified `ParseNode` to `this` node to represent a
  /// return type.
  /// @param parameter_node A `ParseNode` representing a lambda return type.
  /// @warning This method attaches its argument using
  /// `ParseNode::AppendChild`, appending it to the end of the children vector
  /// field.
  void AddReturnTypeNode(std::unique_ptr<ParseNode> return_type_node);

  /// @brief Attach the specified `ParseNode` to `this` node to represent a
  /// lambda body.
  /// @param parameter_node A `ParseNode` representing a lambda body.
  /// @warning This method attaches its argument using
  /// `ParseNode::AppendChild`, appending it to the end of the children vector
  /// field.
  void AddLambdaBodyNode(std::unique_ptr<ParseNode> lambda_body);

  std::vector<LambdaParameterNode *> GetParameterNodes() const {
    return parameter_nodes;
  }
  ParseNode *GetReturnTypeNode() const { return return_type_node; }
  CodeBlockNode *GetLambdaBodyNode() const {
    return dynamic_cast<CodeBlockNode *>(lambda_body);
  }

  bool IsAnonymous() const;

  /// @brief Checks if this lambda node represents a proper function (i.e. a
  /// variable defined using `function function_name ...`).
  /// @return `true` if this lambda is a proper function, `false` otherwise.
  bool IsFunction() const {
    return GetParent()->GetParseNodeType() ==
           ParseNodeType::FUNCTION_DEFINITION_NODE;
  }

  virtual std::string ToString() const override { return "LAMBDA NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  std::vector<LambdaParameterNode *> parameter_nodes;
  ParseNode *return_type_node;
  ParseNode *lambda_body;
};

#endif  // LAMBDA_NODE_H_
