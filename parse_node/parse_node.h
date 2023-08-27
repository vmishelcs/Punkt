#ifndef PARSE_NODE_H_
#define PARSE_NODE_H_

#include <memory>
#include <experimental/memory>

#include <token/token.h>

class ParseNode {
public:
    ParseNode(std::unique_ptr<Token> token) : token(std::move(token)), parent{nullptr} {}

    void AppendChild(std::unique_ptr<ParseNode> node) {
        node->parent = this;
        children.push_back(std::move(node));
    }

    virtual std::string GetNodeString() { return "PARSE NODE: " + token->GetTokenString(); }

protected:
    std::unique_ptr<Token> token;

private:
    ParseNode *parent;
    std::vector<std::unique_ptr<ParseNode>> children;
};

#endif // PARSE_NODE_H_