#include <logging/punkt_logger.h>

#include "scope.h"

Scope::Scope(ScopeType scope_type, Scope *base_scope)
    : scope_type(scope_type)
    , base_scope(base_scope)
    , symbol_table()
{}

std::unique_ptr<Scope> Scope::CreateGlobalScope() {
    return std::make_unique<Scope>(ScopeType::GLOBAL_SCOPE);
}

std::unique_ptr<Scope> Scope::CreateParameterScope() {
    return std::make_unique<Scope>(ScopeType::PARAMETER_SCOPE, this);
}

std::unique_ptr<Scope> Scope::CreateProcedureScope() {
    return std::make_unique<Scope>(ScopeType::PROCEDURE_SCOPE, this);
}

std::unique_ptr<Scope> Scope::CreateSubscope() {
    return std::make_unique<Scope>(ScopeType::SUBSCOPE, this);
}

void Scope::Declare(const std::string& symbol,
        const TextLocation& tl,
        bool is_mutable,
        Type *type,
        SymbolType symbol_type)
{
    symbol_table.Insert(
        symbol,
        tl,
        is_mutable,
        type,
        symbol_type
    );
}

bool Scope::Declares(const std::string& symbol) {
    return symbol_table.Contains(symbol);
}

Scope *Scope::GetBaseScope() const {
    return base_scope;
}

SymbolTableEntry& Scope::GetSymbolTableEntry(const std::string& symbol) {
    return symbol_table.Get(symbol);
}

ScopeType Scope::GetScopeType() const {
    return scope_type;
}

std::string Scope::GetAttributeString() const {
    return GetScopeTypeString(scope_type);
}

std::string Scope::GetScopeTypeString(ScopeType scope_type) {
    switch (scope_type) {
        case ScopeType::GLOBAL_SCOPE:
            return "global scope";
        case ScopeType::PARAMETER_SCOPE:
            return "parameter scope";
        case ScopeType::PROCEDURE_SCOPE:
            return "procedure scope";
        case ScopeType::SUBSCOPE:
            return "subscope";
        default:
            return (const char *)PunktLogger::LogFatalInternalError(
                    "unimplemented ScopeType in Scope::GetScopeTypeString");
    }
}
