#ifndef PARSE_NODE_H_
#define PARSE_NODE_H_

#include <memory>
#include <vector>
#include <optional>

#include <llvm/IR/Value.h>

#include <semantic_analyzer/types/type.h>
#include <symbol_table/scope.h>
#include <token/token.h>

// Forward-declare ParseNodeVisitor to avoid circular dependencies
class ParseNodeVisitor;

// Forward-declare ParseNodeIRVisitor to avoid circular dependencies
class ParseNodeIRVisitor;

enum class ParseNodeType {
    ASSIGNMENT_STATEMENT_NODE,
    BOOLEAN_LITERAL_NODE,
    CHARACTER_LITERAL_NODE,
    CODE_BLOCK_NODE,
    DECLARATION_STATEMENT_NODE,
    ERROR_NODE,
    FOR_STATEMENT_NODE,
    FUNCTION_NODE,
    FUNCTION_PROTOTYPE_NODE,
    FUNCTION_PARAMETER_NODE,
    IDENTIFIER_NODE,
    IF_STATEMENT_NODE,
    INTEGER_LITERAL_NODE,
    MAIN_NODE,
    OPERATOR_NODE,
    PRINT_STATEMENT_NODE,
    PROGRAM_NODE,
    RETURN_STATEMENT_NODE,
    STRING_LITERAL_NODE,
    TYPE_NODE
};

class ParseNode {
public:
    ParseNode(ParseNodeType node_type, std::unique_ptr<Token> token);

    ParseNodeType GetParseNodeType() const;

    Token *GetToken() const;

    ParseNode *GetParent() const;
    unsigned NumChildren() const;
    ParseNode *GetChild(unsigned i) const;
    std::vector<ParseNode *> GetChildren() const;
    void AppendChild(std::unique_ptr<ParseNode> node);

    std::vector<ParseNode *> GetPathToRoot();

    virtual std::string ToString() const = 0;

    virtual void Accept(ParseNodeVisitor& visitor) = 0;

    bool HasType() const;
    Type *GetType() const;
    void SetType(std::unique_ptr<Type> type);

    bool HasScope() const;
    Scope *GetScope() const;
    void SetScope(std::unique_ptr<Scope> scope);

    Scope *GetLocalScope();
    bool ScopeDeclares(const std::string& identifier);
    SymbolTableEntry& GetDeclarationData(const std::string& identifier);

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) = 0;

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
