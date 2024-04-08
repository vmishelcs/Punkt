#ifndef NOP_NODE_H_
#define NOP_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

class NopNode : public ParseNode {
public:
    NopNode();

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // NOP_NODE_H_
