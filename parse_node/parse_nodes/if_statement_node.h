#ifndef IF_STATEMENT_NODE_H_
#define IF_STATEMENT_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

class IfStatementNode : public ParseNode {
public:
    IfStatementNode(std::unique_ptr<Token> token);

    bool HasElseBlock() const;

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // IF_STATEMENT_NODE_H_
