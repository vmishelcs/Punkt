#ifndef INTEGER_LITERAL_NODE_H_
#define INTEGER_LITERAL_NODE_H_

#include "parse_node.h"

class IntegerLiteralNode : public ParseNode {
public:
    IntegerLiteralNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {}
    virtual std::string GetNodeString() override {
        return "INTEGER LITERAL NODE: " + token->GetTokenString();
    }
};

#endif // INTEGER_LITERAL_NODE_H_
