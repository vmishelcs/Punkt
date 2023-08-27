#ifndef PROGRAM_NODE_H_
#define PROGRAM_NODE_H_

#include "parse_node.h"

class ProgramNode : public ParseNode {
public:
    ProgramNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {}
    virtual std::string GetNodeString() { return "PROGRAM NODE: " + token->GetTokenString(); }
};

#endif // PROGRAM_NODE_H_
