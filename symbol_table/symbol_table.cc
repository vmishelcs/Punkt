#include "symbol_table.h"

#include <logging/punkt_logger.h>

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>

SymbolTableEntry* SymbolTable::Insert(const std::string& symbol,
                                      const TextLocation& tl, bool is_mutable,
                                      Type* type, SymbolType symbol_type) {
  if (Contains(symbol)) {
    SymbolRedefinitionError(symbol, tl);
    return nullptr;
  }

  auto entry = std::make_unique<SymbolTableEntry>(
      TextLocation(tl), is_mutable, type, symbol_type, nullptr, nullptr);
  const auto& [it, inserted] =
      table.insert({std::string(symbol), std::move(entry)});
  assert(inserted && "failed to insert new symbol table entry");
  return it->second.get();
}

SymbolTableEntry* SymbolTable::Get(const std::string& symbol) {
  return table[symbol].get();
}

bool SymbolTable::Contains(const std::string& symbol) const {
  return table.contains(symbol);
}

void SymbolTable::UndefinedSymbolReference(const std::string& symbol,
                                           const TextLocation& tl) {
  std::string message =
      "Reference to undefined symbol \'" + symbol + "\' at \n";
  message += ("\t" + tl.ToString());
  PunktLogger::Log(LogType::SYMBOL_TABLE, message);
}

void SymbolTable::SymbolRedefinitionError(const std::string& symbol,
                                          const TextLocation& tl) {
  std::string message = "Redefinition of symbol \'" + symbol + "\' at \n";
  message += ("\t" + tl.ToString() + "\n");
  message += ("previously defined at \n");
  message += ("\t" + table[symbol]->text_location.ToString());
  PunktLogger::Log(LogType::SYMBOL_TABLE, message);
}
