#include "symbol_table.h"

void SymbolTable::Insert(std::string symbol, const TextLocation& tl, bool is_mutable, TypeEnum type_enum) {
    if (Contains(symbol)) {

    }
    else {
        table.insert({symbol, {
            .is_mutable = is_mutable,
            .type_enum = type_enum,
            .text_location = tl
        }});
    }
}

SymbolData SymbolTable::GetSymbolData(std::string symbol) {
    return table.at(symbol);
}

bool SymbolTable::Contains(std::string symbol) const {
    return table.contains(symbol);
}

void SymbolTable::SymbolRedefinitionError(std::string symbol, SymbolData symbol_data) {

}
