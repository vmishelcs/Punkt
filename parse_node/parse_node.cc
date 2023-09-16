#include "parse_node.h"
#include "parse_node_visitor.h"

ParseNode::ParseNode(ParseNodeType node_type, std::unique_ptr<Token> token)
    : token(std::move(token))
    , node_type(node_type)
    , parent{nullptr}
    , type{nullptr}
    , scope{nullptr}
{}

ParseNodeType ParseNode::GetNodeType() const {
    return node_type;
}

Token& ParseNode::GetToken() const {
    return *token;
}

ParseNode& ParseNode::GetParent() {
    return *parent;
}

ParseNode& ParseNode::GetChild(int i) {
    return *children[i];
}

std::vector<std::unique_ptr<ParseNode>>& ParseNode::GetChildren() {
    return children;
}

void ParseNode::AppendChild(std::unique_ptr<ParseNode> node) {
    node->parent = this;
    children.push_back(std::move(node));
}

std::vector<ParseNode *> ParseNode::GetPathToRoot() {
    std::vector<ParseNode *> result;
    ParseNode *current = this;
    while (current) {
        result.push_back(current);
        current = current->parent;
    }
    return result;
}

bool ParseNode::HasType() const {
    return type != nullptr;
}

Type& ParseNode::GetType() const {
    return *type;
}

void ParseNode::SetType(std::unique_ptr<Type> type) {
    this->type = std::move(type);
}

bool ParseNode::HasScope() const {
    return scope != nullptr;
}

Scope& ParseNode::GetScope() const {
    return *scope;
}

void ParseNode::SetScope(std::unique_ptr<Scope> scope) {
    this->scope = std::move(scope);
}

std::optional<std::reference_wrapper<Scope>> ParseNode::GetLocalScope() {
    for (ParseNode *node : GetPathToRoot()) {
        if (node->HasScope()) {
            return node->GetScope();
        }
    }

    return std::nullopt;
}

bool ParseNode::ScopeDeclares(const std::string& identifier) {
    if (!HasScope()) {
        return false;
    }
    return scope->Declares(identifier);
}

SymbolData& ParseNode::GetDeclarationData(const std::string& identifier) {
    return scope->GetSymbolData(identifier);
}

void ParseNode::VisitChildren(ParseNodeVisitor& visitor) {
    for (auto& child : children) {
        child->Accept(visitor);
    }
}
