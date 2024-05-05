#ifndef CALL_STATEMENT_NODE_H_
#define CALL_STATEMENT_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>

#include <memory>
#include <string>

#include "lambda_invocation_node.h"

class CallStatementNode : public ParseNode {
 public:
  CallStatementNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::CALL_STATEMENT_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  LambdaInvocationNode *GetLambdaInvocationNode() const {
    return dynamic_cast<LambdaInvocationNode *>(GetChild(0));
  }

  virtual std::string ToString() const override {
    return "CALL STATEMENT NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // CALL_STATEMENT_NODE_H_
