#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/identifier_token.h>

#include "identifier_node.h"

IdentifierNode::IdentifierNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::IDENTIFIER_NODE, std::move(token))
{}

std::string IdentifierNode::GetName() const {
    IdentifierToken& identifier_token = dynamic_cast<IdentifierToken&>(*token);
    return identifier_token.GetLexeme();
}

std::string IdentifierNode::ToString() const {
    return "IDENTIFIER NODE: " + token->ToString();
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

llvm::Value *IdentifierNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
