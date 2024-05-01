#ifndef LAMBDA_INVOCATION_NODE_H_
#define LAMBDA_INVOCATION_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>
#include <vector>

#include "identifier_node.h"
#include "lambda_node.h"

class LambdaInvocationNode : public ParseNode {
 public:
  LambdaInvocationNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::LAMBDA_INVOCATION_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  ParseNode *GetCalleeNode() const { return GetChild(0); }

  std::vector<ParseNode *> GetArgumentNodes() const;

  virtual std::string ToString() const override {
    return "LAMBDA INVOCATION NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // LAMBDA_INVOCATION_NODE_H_
