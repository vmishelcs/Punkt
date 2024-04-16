#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/token.h>

#include "identifier_node.h"

IdentifierNode::IdentifierNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::IDENTIFIER_NODE, std::move(token))
    , name(std::string(this->token->GetLexeme()))
{}

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

void IdentifierNode::SetLLVMAlloca(llvm::AllocaInst *alloca) {
    auto symbol_table_entry_opt = FindSymbolTableEntry();
    if (!symbol_table_entry_opt.has_value()) {
        PunktLogger::LogFatalInternalError(
                "could not find symbol table entry in IdentifierNode::SetLLVMAlloca");
    }
    symbol_table_entry_opt.value().get().alloca = alloca;
}

void IdentifierNode::SetLLVMFunction(llvm::Function *function) {
    auto symbol_table_entry_opt = FindSymbolTableEntry();
    if (!symbol_table_entry_opt.has_value()) {
        PunktLogger::LogFatalInternalError(
                "could not find symbol table entry in IdentifierNode::SetLLVMFunction");
    }
    symbol_table_entry_opt.value().get().function = function;
}

llvm::AllocaInst *IdentifierNode::FindLLVMAlloca() {
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

llvm::Function *IdentifierNode::FindLLVMFunction() {
    std::string identifier = this->GetName();
    Scope *scope = this->GetLocalScope();
    while (scope) {
        if (scope->Declares(identifier) && scope->GetSymbolTableEntry(identifier).function) {
            return scope->GetSymbolTableEntry(identifier).function;
        }
        scope = scope->GetBaseScope();
    }
    return nullptr;
}

llvm::Value *IdentifierNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
