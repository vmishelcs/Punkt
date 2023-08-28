#ifndef DECLARATION_NODE_H_
#define DECLARATION_NODE_H_

#include "parse_node.h"

class DeclarationNode : public ParseNode {
public:
    DeclarationNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {}
    virtual std::string GetNodeString() { return "DECLARATION NODE: " + token->GetTokenString(); }
};

#endif // DECLARATION_NODE_H_