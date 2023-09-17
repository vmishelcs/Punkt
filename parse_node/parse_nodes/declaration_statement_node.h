#ifndef DECLARATION_NODE_H_
#define DECLARATION_NODE_H_

#include <parse_node/parse_node.h>

class DeclarationStatementNode : public ParseNode {
public:
    DeclarationStatementNode(std::unique_ptr<Token> token);

    virtual std::string GetNodeString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;
};

#endif // DECLARATION_NODE_H_
