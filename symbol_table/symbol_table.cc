#include <logging/punkt_logger.h>

#include "symbol_table.h"


void SymbolTable::Insert(const std::string& symbol,
        const TextLocation& tl,
        bool is_mutable,
        Type *type,
        SymbolType symbol_type)
{
    if (Contains(symbol)) {
        SymbolRedefinitionError(symbol, tl);
    }
    else {
        table.insert({std::string(symbol), 
            {
                .text_location = TextLocation(tl),
                .is_mutable = is_mutable,
                .type = type,
                .symbol_type = symbol_type,
                .alloca = nullptr,
                .function = nullptr
            }
        });
    }
}

SymbolTableEntry& SymbolTable::Get(const std::string& symbol) {
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
