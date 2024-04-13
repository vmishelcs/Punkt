#ifndef FUNCTION_PARAMETER_NODE_H_
#define FUNCTION_PARAMETER_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

#include "identifier_node.h"
#include "type_node.h"

class FunctionParameterNode : public ParseNode {
public:
    FunctionParameterNode();

    static std::unique_ptr<FunctionParameterNode> CreateParameterNode(
            std::unique_ptr<ParseNode> type,
            std::unique_ptr<ParseNode> identifier);

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // FUNCTION_PARAMETER_NODE_H_
