#ifndef SCOPE_H_
#define SCOPE_H_

#include <string>
#include <memory>

#include "symbol_table.h"

enum class ScopeType {
    GLOBAL_SCOPE,
    PARAMETER_SCOPE,
    PROCEDURE_SCOPE,
    SUBSCOPE,
    NULL_SCOPE
};

class Scope {
public:
    Scope(ScopeType scope_type, Scope *base_scope = nullptr);

    static std::unique_ptr<Scope> CreateGlobalScope();
    std::unique_ptr<Scope> CreateParameterScope();
    std::unique_ptr<Scope> CreateProcedureScope();
    std::unique_ptr<Scope> CreateSubscope();

    void Declare(const std::string& symbol, const TextLocation& tl, bool is_mutable, Type *type);
    bool Declares(const std::string& symbol);

    Scope *GetBaseScope() const;

    SymbolTableEntry& GetSymbolTableEntry(const std::string& symbol);

    ScopeType GetScopeType() const;

    std::string GetAttributeString() const;
    static std::string GetScopeTypeString(ScopeType scope_type);

private:

    ScopeType scope_type;
    Scope *base_scope;
    SymbolTable symbol_table;
};

#endif // SCOPE_H_
