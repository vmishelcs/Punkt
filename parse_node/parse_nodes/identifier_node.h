#ifndef IDENTIFIER_NODE_H_
#define IDENTIFIER_NODE_H_

#include <parse_node/parse_node.h>

class IdentifierNode : public ParseNode {
public:
    IdentifierNode(std::unique_ptr<Token> token);

    virtual std::string GetNodeString() override;

    virtual void Accept(ParseNodeVisitor& visitor) override;
};

#endif // IDENTIFIER_NODE_H_
