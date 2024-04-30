#ifndef LAMBDA_INVOCATION_NODE_H_
#define LAMBDA_INVOCATION_NODE_H_

#include <parse_node/parse_node.h>

#include <memory>
#include <string>
#include <vector>

#include "identifier_node.h"
#include "lambda_node.h"

class LambdaInvocationNode : public ParseNode {
 public:
  LambdaInvocationNode(TextLocation text_location)
      : ParseNode(ParseNodeType::LAMBDA_INVOCATION_NODE, text_location) {}

  ParseNode *GetCalleeNode() const { return GetChild(0); }

  std::vector<ParseNode *> GetArgumentNodes() const;

  virtual std::string ToString() const override {
    return "LAMBDA INVOCATION NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // LAMBDA_INVOCATION_NODE_H_
