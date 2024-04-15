#ifndef LAMBDA_INVOCATION_NODE_H_
#define LAMBDA_INVOCATION_NODE_H_

#include <memory>
#include <string>
#include <vector>

#include <parse_node/parse_node.h>

#include "identifier_node.h"

class LambdaInvocationNode : public ParseNode {
public:
    LambdaInvocationNode()
        : ParseNode(ParseNodeType::LAMBDA_INVOCATION_NODE, nullptr) 
    {}

    static std::unique_ptr<LambdaInvocationNode> CreateLambdaInvocationNodeWithArguments(
        std::unique_ptr<ParseNode> identifier,
        std::vector<std::unique_ptr<ParseNode>> args
    );

    IdentifierNode *GetIdentifierNode() const {
        return dynamic_cast<IdentifierNode *>(GetChild(0));
    }

    std::vector<ParseNode *> GetArgumentNodes() const;

    virtual std::string ToString() const override { return "LAMBDA INVOCATION NODE"; }

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // LAMBDA_INVOCATION_NODE_H_
