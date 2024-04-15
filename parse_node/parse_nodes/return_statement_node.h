#ifndef RETURN_STATEMENT_NODE_H_
#define RETURN_STATEMENT_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

#include "lambda_node.h"

class ReturnStatementNode : public ParseNode {
public:
    ReturnStatementNode(std::unique_ptr<Token> token);

    /// @brief Get the `LambdaNode` that represents the enclosing lambda.
    /// @return `LambdaNode` pointer representing the lambda from which this return statement is
    /// returning from.
    LambdaNode *GetEnclosingLambdaNode() const;

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // RETURN_STATEMENT_NODE_H_
