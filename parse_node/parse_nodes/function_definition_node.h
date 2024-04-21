#ifndef FUNCTION_DECLARATION_NODE_H_
#define FUNCTION_DECLARATION_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include "identifier_node.h"
#include "lambda_node.h"

class FunctionDefinitionNode : public ParseNode {
 public:
  FunctionDefinitionNode(std::unique_ptr<Token> token);

  IdentifierNode *GetIdentifierNode() const {
    return dynamic_cast<IdentifierNode *>(GetChild(0));
  }

  LambdaNode *GetLambdaNode() const {
    return dynamic_cast<LambdaNode *>(GetChild(1));
  }

  virtual std::string ToString() const override {
    return "FUNCTION DEFINITION NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // FUNCTION_DECLARATION_NODE_H_
