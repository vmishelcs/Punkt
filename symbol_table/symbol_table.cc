#include "symbol_table.h"

#include <logging/punkt_logger.h>

void SymbolTable::Insert(const std::string& symbol, const TextLocation& tl, bool is_mutable, const Type& type) {
    if (Contains(symbol)) {
        SymbolRedefinitionError(symbol, tl);
    }
    else {
        table.insert({symbol, {
            .text_location = tl,
            .is_mutable = is_mutable,
            .type = type,
            .binding = nullptr
        }});
    }
}

SymbolData& SymbolTable::Get(const std::string& symbol) {
    return table.at(symbol);
}

bool SymbolTable::Contains(const std::string& symbol) const {
    return table.contains(symbol);
}

void SymbolTable::UndefinedSymbolReference(const std::string& symbol, const TextLocation& tl) {
    std::string message = "Reference to undefined symbol \'" + symbol + "\' at \n";
    message += ("\t" + tl.ToString());
    PunktLogger::Log(LogType::SYMBOL_TABLE, message);
}

void SymbolTable::SymbolRedefinitionError(const std::string& symbol, const TextLocation& tl) {
    std::string message = "Redefinition of symbol \'" + symbol + "\' at \n";
    message += ("\t" + tl.ToString() + "\n");
    message += ("previously defined at \n");
    message += ("\t" + table.at(symbol).text_location.ToString());
    PunktLogger::Log(LogType::SYMBOL_TABLE, message);
}
