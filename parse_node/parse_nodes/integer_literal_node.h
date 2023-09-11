#ifndef INTEGER_LITERAL_NODE_H_
#define INTEGER_LITERAL_NODE_H_

#include <parse_node/parse_node.h>

class IntegerLiteralNode : public ParseNode {
public:
    IntegerLiteralNode(std::unique_ptr<Token> token);

    virtual std::string GetNodeString() override;

    virtual void Accept(ParseNodeVisitor& visitor) override;
};

#endif // INTEGER_LITERAL_NODE_H_
