#ifndef PARSE_NODE_H_
#define PARSE_NODE_H_

#include <memory>

#include <token/token.h>

class ParseNode {
public:
    ParseNode(Token& token) : token(token) {}

private:
    Token& token;
    
    std::vector<std::unique_ptr<ParseNode>> children;
};

#endif // PARSE_NODE_H_
