#ifndef ASSIGNMENT_STATEMENT_NODE_H_
#define ASSIGNMENT_STATEMENT_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

class AssignmentStatementNode : public ParseNode {
public:
    AssignmentStatementNode();

    ParseNode *GetTargetNode() const {
        return GetChild(0);
    }

    ParseNode *GetNewValueNode() const {
        return GetChild(1);
    }

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // ASSIGNMENT_STATEMENT_NODE_H_
