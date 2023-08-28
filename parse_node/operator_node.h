#ifndef OPERATOR_NODE_H_
#define OPERATOR_NODE_H_

#include <scanner/punctuator.h>
#include <token/punctuator_token.h>

#include "parse_node.h"

class OperatorNode : public ParseNode {
public:
    OperatorNode(std::unique_ptr<Token> token) : ParseNode(std::move(token)) {
        PunctuatorToken& punctuator_token = dynamic_cast<PunctuatorToken&>(*(this->token));
        punctuator_enum = punctuator_token.GetPunctuatorEnum();
    }
    virtual std::string GetNodeString() { return "OPERATOR NODE: " + token->GetTokenString(); }

private:
    PunctuatorEnum punctuator_enum;
};

#endif // OPERATOR_NODE_H_
