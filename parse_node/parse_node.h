#ifndef PARSE_NODE_H_
#define PARSE_NODE_H_

#include <memory>
#include <vector>

#include <token/token.h>
#include <semantic_analyzer/type.h>
#include <symbol_table/scope.h>

// Forward-declare ParseNodeVisitor to avoid circular dependencies
class ParseNodeVisitor;

class ParseNode {
public:
    ParseNode(std::unique_ptr<Token> token);

    Token& GetToken() const;

    ParseNode& GetChild(int i);
    std::vector<std::unique_ptr<ParseNode>>& GetChildren();
    void AppendChild(std::unique_ptr<ParseNode> node);

    std::vector<ParseNode *> GetPathToRoot();

    virtual std::string GetNodeString() = 0;

    virtual void Accept(ParseNodeVisitor& visitor) = 0;

    bool HasType() const;
    Type& GetType() const;
    void SetType(std::unique_ptr<Type> type);

    bool HasScope() const;
    Scope& GetScope() const;
    void SetScope(std::unique_ptr<Scope> scope);
    Scope& GetLocalScope();

protected:
    void VisitChildren(ParseNodeVisitor& visitor);

    std::unique_ptr<Token> token;

private:
    ParseNode *parent;
    std::vector<std::unique_ptr<ParseNode>> children;
    std::unique_ptr<Type> type; // Type that describes this node, if any
    std::unique_ptr<Scope> scope; // Scope created by this node; not every node creates a scope
};

#endif // PARSE_NODE_H_
