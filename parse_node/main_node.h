#ifndef MAIN_NODE_H_
#define MAIN_NODE_H_

#include "parse_node.h"

class MainNode : public ParseNode {
public:
    MainNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {}
    virtual std::string GetNodeString() override {
        return "MAIN NODE: " + token->GetTokenString();
    }
};

#endif // MAIN_NODE_H_
