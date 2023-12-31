#ifndef PARSE_NODE_H_
#define PARSE_NODE_H_

#include <memory>
#include <vector>
#include <optional>

#include <token/token.h>
#include <semantic_analyzer/type.h>
#include <symbol_table/scope.h>

// Forward-declare ParseNodeVisitor to avoid circular dependencies
class ParseNodeVisitor;

enum class ParseNodeType {
    CODE_BLOCK_NODE,
    DECLARATION_STATEMENT_NODE,
    ERROR_NODE,
    IDENTIFIER_NODE,
    INTEGER_LITERAL_NODE,
    MAIN_NODE,
    OPERATOR_NODE,
    PRINT_STATEMENT_NODE,
    PROGRAM_NODE
};

class ParseNode {
public:
    ParseNode(ParseNodeType node_type, std::unique_ptr<Token> token);

    ParseNodeType GetNodeType() const;

    Token& GetToken() const;

    ParseNode& GetParent() const;
    ParseNode& GetChild(int i) const;
    std::vector<std::reference_wrapper<ParseNode>> GetChildren() const;
    void AppendChild(std::unique_ptr<ParseNode> node);

    std::vector<ParseNode *> GetPathToRoot();

    virtual std::string AsString() const = 0;

    virtual void Accept(ParseNodeVisitor& visitor) = 0;

    bool HasType() const;
    Type& GetType() const;
    void SetType(std::unique_ptr<Type> type);

    bool HasScope() const;
    Scope& GetScope() const;
    void SetScope(std::unique_ptr<Scope> scope);
    std::optional<std::reference_wrapper<Scope>> GetLocalScope();
    bool ScopeDeclares(const std::string& identifier);
    SymbolData& GetDeclarationData(const std::string& identifier);

protected:
    void VisitChildren(ParseNodeVisitor& visitor);

    std::unique_ptr<Token> token;

private:
    ParseNodeType node_type;
    ParseNode *parent;
    std::vector<std::unique_ptr<ParseNode>> children;
    std::unique_ptr<Type> type; // Type that describes this node, if any
    std::unique_ptr<Scope> scope; // Scope created by this node; not every node creates a scope
};

#endif // PARSE_NODE_H_
