#ifndef MAIN_NODE_H_
#define MAIN_NODE_H_

#include <parse_node/parse_node.h>

class MainNode : public ParseNode {
public:
    MainNode(std::unique_ptr<Token> token);

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(CodeGenerationVisitor& visitor) override;
};

#endif // MAIN_NODE_H_
