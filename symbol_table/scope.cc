#include "scope.h"

Scope::Scope(ScopeType scope_type, Scope *base_scope)
    : scope_type(scope_type)
    , base_scope(base_scope)
    , symbol_table()
{}

std::unique_ptr<Scope> Scope::CreateGlobalScope() {
    return std::unique_ptr<Scope>(new Scope(ScopeType::GLOBAL_SCOPE));
}

std::unique_ptr<Scope> Scope::CreateSubscope() {
    return std::unique_ptr<Scope>(new Scope(ScopeType::SUBSCOPE, this));
}

void Scope::DeclareInScope(const std::string& symbol, const TextLocation& tl, bool is_mutable, const Type& type) {
    symbol_table.Insert(
        symbol,
        tl,
        is_mutable,
        type
    );
}

bool Scope::Declares(const std::string& symbol) {
    return symbol_table.Contains(symbol);
}

SymbolData& Scope::GetSymbolData(const std::string& symbol) {
    return symbol_table.Get(symbol);
}

ScopeType Scope::GetScopeType() const {
    return scope_type;
}

std::string Scope::ToString() const {
    std::string result = "[Scope: ";
    switch (scope_type) {
        case ScopeType::GLOBAL_SCOPE:
            result.append("GLOBAL SCOPE");
            break;
        case ScopeType::SUBSCOPE:
            result.append("SUBSCOPE");
            break;
        default:
            result.append("UNIMPLEMENTED SCOPE");
    }
    result.push_back(']');
    return result;
}
