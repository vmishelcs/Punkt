#ifndef FOR_STATEMENT_NODE_H_
#define FOR_STATEMENT_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

class ForStatementNode : public ParseNode {
public:
    ForStatementNode(std::unique_ptr<Token> token);

    ParseNode *GetLoopInitializerNode() const { return GetChild(0); }
    ParseNode *GetEndConditionNode() const { return GetChild(1); }
    ParseNode *GetStepValueNode() const { return GetChild(2); }
    ParseNode *GetLoopBodyNode() const { return GetChild(3); }

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // FOR_STATEMENT_NODE_H_
