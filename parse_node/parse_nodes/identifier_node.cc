#include <parse_node/parse_node_visitor.h>

#include "identifier_node.h"

IdentifierNode::IdentifierNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::IDENTIFIER_NODE, std::move(token))
{}

std::string IdentifierNode::AsString() const {
    return "IDENTIFIER NODE: " + token->AsString();
}

void IdentifierNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}

std::optional<std::reference_wrapper<const SymbolData>> IdentifierNode::FindIdentifierSymbolData() {
    std::string identifier = token->GetLexeme();
    for (ParseNode *node : GetPathToRoot()) {
        if (node->ScopeDeclares(identifier)) {
            return node->GetDeclarationData(identifier);
        }
    }
    return std::nullopt;
}
