#ifndef PRINT_STATEMENT_NODE_H_
#define PRINT_STATEMENT_NODE_H_

#include "parse_node.h"

class PrintStatementNode : public ParseNode {
public:
    PrintStatementNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {}
    virtual std::string GetNodeString() override {
        return "PRINT STATEMENT NODE: " + token->GetTokenString();
    }
};

#endif // PRINT_STATEMENT_NODE_H_
