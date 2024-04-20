#ifndef RETURN_STATEMENT_NODE_H_
#define RETURN_STATEMENT_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

#include "lambda_node.h"

class ReturnStatementNode : public ParseNode {
public:
    ReturnStatementNode(std::unique_ptr<Token> token);

    /// @brief Get the node that represents the return value of this statement.
    /// @return Pointer to the node that represents the return value of this statement.
    ParseNode *GetReturnValueNode() const {
        return GetChild(0);
    }

    /// @brief Get the node that represents the enclosing function of this return statement.
    /// @return Pointer to the node that represents the enclosing function; this can be a
    /// `LambdaNode` or a `MainNode`. This method returns `nullptr` if no such node could be found.
    ParseNode *GetEnclosingFunctionNode() const;

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // RETURN_STATEMENT_NODE_H_
