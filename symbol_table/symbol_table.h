#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <unordered_map>
#include <string>

#include <input_handler/text_location.h>
#include <semantic_analyzer/type.h>

struct SymbolData {
    bool is_mutable;
    TypeEnum type_enum;
    const TextLocation& text_location;
};

class SymbolTable {
public:
    void Insert(const std::string& symbol, const TextLocation& tl, bool is_mutable, TypeEnum type_enum);
    const SymbolData& GetSymbolData(const std::string& symbol);

    bool Contains(const std::string& symbol) const;

    void UndefinedSymbolReference(const std::string& symbol, const TextLocation& tl);

private:
    void SymbolRedefinitionError(const std::string& symbol, const TextLocation& tl);

    std::unordered_map<std::string, SymbolData> table;
};

#endif // SYMBOL_TABLE_H_
