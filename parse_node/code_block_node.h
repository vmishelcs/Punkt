#ifndef CODE_BLOCK_NODE_H_
#define CODE_BLOCK_NODE_H_

#include "parse_node.h"

class CodeBlockNode : public ParseNode {
public:
    CodeBlockNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {}
    virtual std::string GetNodeString() override {
        return "CODE BLOCK NODE: " + token->GetTokenString();
    }
};

#endif // CODE_BLOCK_NODE_H_
