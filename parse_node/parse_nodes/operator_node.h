#ifndef OPERATOR_NODE_H_
#define OPERATOR_NODE_H_

#include <scanner/punctuator.h>
#include <token/punctuator_token.h>

#include <parse_node/parse_node.h>

class OperatorNode : public ParseNode {
public:
    OperatorNode(std::unique_ptr<Token> token);

    inline PunctuatorEnum GetPunctuatorEnum() { return punctuator_enum; }

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(CodeGenerationVisitor& visitor) override;

private:
    PunctuatorEnum punctuator_enum;
};

#endif // OPERATOR_NODE_H_
