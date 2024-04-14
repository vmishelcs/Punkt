#ifndef FUNCTION_DECLARATION_NODE_H_
#define FUNCTION_DECLARATION_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

#include "function_prototype_node.h"
#include "identifier_node.h"

class FunctionNode : public ParseNode {
public:
    FunctionNode(std::unique_ptr<Token> token);

    IdentifierNode *GetIdentifierNode() const {
        return dynamic_cast<IdentifierNode *>(GetChild(0));
    }
    FunctionPrototypeNode *GetFunctionPrototypeNode() const {
        return dynamic_cast<FunctionPrototypeNode *>(GetChild(1));
    }

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // FUNCTION_DECLARATION_NODE_H_
