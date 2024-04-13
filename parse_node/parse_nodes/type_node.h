#ifndef TYPE_NODE_H_
#define TYPE_NODE_H_

#include <string>

#include <parse_node/parse_node.h>
#include <token/token.h>

class TypeNode : public ParseNode {
public:
    TypeNode(std::unique_ptr<Token> token);

    /// @brief Set `Type` based on the token used to create this `TypeNode`.
    void InferOwnType();

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // TYPE_NODE_H_
