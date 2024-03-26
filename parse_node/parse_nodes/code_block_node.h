#ifndef CODE_BLOCK_NODE_H_
#define CODE_BLOCK_NODE_H_

#include <parse_node/parse_node.h>

class CodeBlockNode : public ParseNode {
public:
    CodeBlockNode(std::unique_ptr<Token> token);

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(CodeGenerationVisitor& visitor) override;
};

#endif // CODE_BLOCK_NODE_H_
