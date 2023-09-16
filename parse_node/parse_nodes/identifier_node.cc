#include <parse_node/parse_node_visitor.h>

#include "identifier_node.h"

IdentifierNode::IdentifierNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::IDENTIFIER_NODE, std::move(token))
{}

std::string IdentifierNode::GetNodeString() {
    return "IDENTIFIER NODE: " + token->GetTokenString();
}

void IdentifierNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}

std::optional<std::reference_wrapper<SymbolData>> IdentifierNode::FindIdentifierSymbolData() {
    std::string identifier = token->GetLexeme();
    for (ParseNode *node : GetPathToRoot()) {
        if (node->ScopeDeclares(identifier)) {
            return node->GetDeclarationData(identifier);
        }
    }
    return std::nullopt;
}
