#ifndef SCOPE_H_
#define SCOPE_H_

#include <string>
#include <memory>

#include "symbol_table.h"

enum class ScopeType {
    GLOBAL_SCOPE,
    SUBSCOPE,
    NULL_SCOPE
};

class Scope {
public:
    static std::unique_ptr<Scope> CreateGlobalScope();
    std::unique_ptr<Scope> CreateSubscope();

    void DeclareInScope(const std::string& symbol, const TextLocation& tl, bool is_mutable, const Type& type);
    bool Declares(const std::string& symbol);

    SymbolData& GetSymbolData(const std::string& symbol);

    ScopeType GetScopeType() const;

    std::string ToString() const;

private:
    Scope(ScopeType scope_type, Scope *base_scope = nullptr);

    ScopeType scope_type;
    Scope *base_scope;
    SymbolTable symbol_table;
};

#endif // SCOPE_H_
