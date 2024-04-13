#ifndef FUNCTION_PROTOTYPE_NODE_H_
#define FUNCTION_PROTOTYPE_NODE_H_

#include <parse_node/parse_node.h>

class FunctionPrototypeNode : public ParseNode {
public:
    FunctionPrototypeNode();

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // FUNCTION_PROTOTYPE_NODE_H_
