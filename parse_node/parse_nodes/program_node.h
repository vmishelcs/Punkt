#ifndef PROGRAM_NODE_H_
#define PROGRAM_NODE_H_

#include <parse_node/parse_node.h>

class ProgramNode : public ParseNode {
public:
    ProgramNode(std::unique_ptr<Token> token);
    
    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;
    
    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // PROGRAM_NODE_H_
