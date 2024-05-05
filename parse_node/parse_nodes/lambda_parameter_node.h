#ifndef LAMBDA_PARAMETER_NODE_H_
#define LAMBDA_PARAMETER_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

#include "identifier_node.h"

class LambdaParameterNode : public ParseNode {
 public:
  LambdaParameterNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::LAMBDA_PARAMETER_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  ParseNode *GetTypeNode() const { return GetChild(0); }

  IdentifierNode *GetIdentifierNode() const {
    return dynamic_cast<IdentifierNode *>(GetChild(1));
  }

  virtual std::string ToString() const override {
    return "LAMBDA PARAMETER NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // LAMBDA_PARAMETER_NODE_H_
