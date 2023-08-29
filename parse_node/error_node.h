#ifndef ERROR_NODE_H_
#define ERROR_NODE_H_

#include "parse_node.h"

class ErrorNode : public ParseNode {
public:
    ErrorNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {}
    virtual std::string GetNodeString() override {
        return "SYNTAX ERROR: " + token->GetTokenString();
    }
};

#endif // ERROR_NODE_H_
