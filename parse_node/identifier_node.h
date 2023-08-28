#ifndef IDENTIFIER_NODE_H_
#define IDENTIFIER_NODE_H_

#include "parse_node.h"

class IdentifierNode : public ParseNode {
public:
    IdentifierNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {}
    virtual std::string GetNodeString() { return "IDENTIFIER NODE: " + token->GetTokenString(); }
};

#endif // IDENTIFIER_NODE_H_
