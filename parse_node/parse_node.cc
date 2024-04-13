#include "parse_node.h"
#include "parse_node_visitor.h"

ParseNode::ParseNode(ParseNodeType node_type, std::unique_ptr<Token> token)
    : token(std::move(token))
    , node_type(node_type)
    , parent{nullptr}
    , type{nullptr}
    , scope{nullptr}
{}

ParseNodeType ParseNode::GetParseNodeType() const {
    return node_type;
}

Token *ParseNode::GetToken() const {
    return token.get();
}

ParseNode *ParseNode::GetParent() const {
    return parent;
}

ParseNode *ParseNode::GetChild(unsigned i) const {
    return children[i].get();
}

unsigned ParseNode::NumChildren() const {
    return children.size();
}

std::vector<ParseNode *> ParseNode::GetChildren() const {
    std::vector<ParseNode *> result;
    for (unsigned i = 0, n = children.size(); i < n; ++i) {
        result.push_back(children[i].get());
    }
    return result;
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

Type *ParseNode::GetType() const {
    return type.get();
}

void ParseNode::SetType(std::unique_ptr<Type> type) {
    this->type = std::move(type);
}

bool ParseNode::HasScope() const {
    return scope != nullptr;
}

Scope *ParseNode::GetScope() const {
    return scope.get();
}

void ParseNode::SetScope(std::unique_ptr<Scope> scope) {
    this->scope = std::move(scope);
}

Scope *ParseNode::GetLocalScope() {
    for (auto node : GetPathToRoot()) {
        if (node->HasScope()) {
            return node->GetScope();
        }
    }
    return nullptr;
}

bool ParseNode::ScopeDeclares(const std::string& identifier) {
    if (!HasScope()) {
        return false;
    }
    return scope->Declares(identifier);
}

SymbolTableEntry& ParseNode::GetDeclarationData(const std::string& identifier) {
    return scope->GetSymbolTableEntry(identifier);
}

void ParseNode::VisitChildren(ParseNodeVisitor& visitor) {
    for (auto& child : children) {
        child->Accept(visitor);
    }
}
