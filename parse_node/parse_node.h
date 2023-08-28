#ifndef PARSE_NODE_H_
#define PARSE_NODE_H_

#include <memory>
#include <vector>

#include <token/token.h>

class ParseNode {
public:
    ParseNode(std::unique_ptr<Token> token) : token(std::move(token)), parent{nullptr} {}

    const std::vector<std::unique_ptr<ParseNode>>& GetChildren() const {
        return children;
    }

    void AppendChild(std::unique_ptr<ParseNode> node) {
        node->parent = this;
        children.push_back(std::move(node));
    }

    virtual std::string GetNodeString() = 0;

protected:
    std::unique_ptr<Token> token;

private:
    ParseNode *parent;
    std::vector<std::unique_ptr<ParseNode>> children;
};

#endif // PARSE_NODE_H_
