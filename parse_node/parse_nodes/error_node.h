#ifndef ERROR_NODE_H_
#define ERROR_NODE_H_

#include <parse_node/parse_node.h>

class ErrorNode : public ParseNode {
public:
    ErrorNode(std::unique_ptr<Token> token);

    virtual std::string AsString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(CodeGenerationVisitor& visitor) override;
};

#endif // ERROR_NODE_H_
