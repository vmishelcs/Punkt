#ifndef FUNCTION_DECLARATION_NODE_H_
#define FUNCTION_DECLARATION_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

class FunctionNode : public ParseNode {
public:
    FunctionNode(std::unique_ptr<Token> token);

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // FUNCTION_DECLARATION_NODE_H_
