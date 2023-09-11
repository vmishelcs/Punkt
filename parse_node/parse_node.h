#ifndef PARSE_NODE_H_
#define PARSE_NODE_H_

#include <memory>
#include <vector>

#include <token/token.h>
#include <semantic_analyzer/type.h>

// Forward-declare ParseNodeVisitor to avoid circular dependencies
class ParseNodeVisitor;

class ParseNode {
public:
    ParseNode(std::unique_ptr<Token> token);

    const std::vector<std::unique_ptr<ParseNode>>& GetChildren() const;

    void AppendChild(std::unique_ptr<ParseNode> node);

    virtual std::string GetNodeString() = 0;

    virtual void Accept(ParseNodeVisitor& visitor) = 0;

protected:
    void VisitChildren(ParseNodeVisitor& visitor);

    std::unique_ptr<Token> token;

private:
    ParseNode *parent;
    std::vector<std::unique_ptr<ParseNode>> children;
};

#endif // PARSE_NODE_H_
