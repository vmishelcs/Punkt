#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

#include "identifier_node.h"

IdentifierNode::IdentifierNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::IDENTIFIER_NODE, std::move(token))
    , name(this->token->GetLexeme())
{}

std::string IdentifierNode::GetName() const {
    return name;
}

std::string IdentifierNode::ToString() const {
    return "IDENTIFIER NODE: " + token->ToString();
}

void IdentifierNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}

// Type *IdentifierNode::FindType() {
//     std::string identifier = this->GetName();
//     Scope *scope = this->GetLocalScope();
//     while (scope) {
//         if (scope->Declares(identifier) && scope->GetSymbolTableEntry(identifier).type) {
//             return scope->GetSymbolTableEntry(identifier).type;
//         }
//         scope = scope->GetBaseScope();
//     }
//     return nullptr;
// }

std::optional<std::reference_wrapper<SymbolTableEntry>> IdentifierNode::FindSymbolTableEntry() {
    std::string identifier = this->GetName();
    Scope *scope = this->GetLocalScope();
    while (scope) {
        if (scope->Declares(identifier)) {
            return scope->GetSymbolTableEntry(identifier);
        }
        scope = scope->GetBaseScope();
    }
    return std::nullopt;
}

llvm::AllocaInst *IdentifierNode::FindAlloca() {
    std::string identifier = this->GetName();
    Scope *scope = this->GetLocalScope();
    while (scope) {
        if (scope->Declares(identifier) && scope->GetSymbolTableEntry(identifier).alloca) {
            return scope->GetSymbolTableEntry(identifier).alloca;
        }
        scope = scope->GetBaseScope();
    }
    return nullptr;
}

llvm::Value *IdentifierNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
