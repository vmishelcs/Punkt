#include "symbol_table.h"

#include <logging/punkt_logger.h>

void SymbolTable::Insert(const std::string& symbol, const TextLocation& tl, bool is_mutable, TypeEnum type_enum) {
    if (Contains(symbol)) {
        SymbolRedefinitionError(symbol, tl);
    }
    else {
        table.insert({symbol, {
            .is_mutable = is_mutable,
            .type_enum = type_enum,
            .text_location = tl
        }});
    }
}

const SymbolData& SymbolTable::GetSymbolData(const std::string& symbol) {
    return table.at(symbol);
}

bool SymbolTable::Contains(const std::string& symbol) const {
    return table.contains(symbol);
}

void SymbolTable::UndefinedSymbolReference(const std::string& symbol, const TextLocation& tl) {
    auto& logger = PunktLogger::GetInstance();
    std::string message = "Reference to undefined symbol \'" + symbol + "\' at \n";
    message += ("\t" + tl.GetString());
    logger.Log(LogType::SYMBOL_TABLE, message);
}

void SymbolTable::SymbolRedefinitionError(const std::string& symbol, const TextLocation& tl) {
    auto& logger = PunktLogger::GetInstance();
    std::string message = "Redefinition of symbol \'" + symbol + "\' at \n";
    message += ("\t" + tl.GetString() + "\n");
    message += ("previously defined at \n");
    message += ("\t" + table.at(symbol).text_location.GetString());
    logger.Log(LogType::SYMBOL_TABLE, message);
}
