#ifndef BOOLEAN_LITERAL_NODE_H_
#define BOOLEAN_LITERAL_NODE_H_

#include <parse_node/parse_node.h>

class BooleanLiteralNode : public ParseNode {
public:
    BooleanLiteralNode(std::unique_ptr<Token> token);

    bool GetValue() const;

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // BOOLEAN_LITERAL_NODE_H_
