#include "symbol_table.h"

#include <logging/punkt_logger.h>

#include <cassert>
#include <memory>
#include <string>

static void SymbolRedefinitionError(const TextLocation &, const std::string &);

SymbolTableEntry *SymbolTable::Insert(const std::string &symbol,
                                      const TextLocation &tl, bool is_mutable,
                                      Type *type, SymbolType symbol_type) {
  if (Contains(symbol)) {
    SymbolRedefinitionError(tl, symbol);
    return nullptr;
  }

  auto entry = std::make_unique<SymbolTableEntry>(
      TextLocation(tl), is_mutable, type, symbol_type, nullptr, nullptr);
  const auto &[it, inserted] =
      table.insert({std::string(symbol), std::move(entry)});
  assert(inserted && "failed to insert new symbol table entry");
  return it->second.get();
}

SymbolTableEntry *SymbolTable::Get(const std::string &symbol) {
  return table[symbol].get();
}

bool SymbolTable::Contains(const std::string &symbol) const {
  return table.contains(symbol);
}

void SymbolRedefinitionError(const TextLocation &text_location,
                             const std::string &symbol) {
  PunktLogger::LogCompileError(text_location,
                               "redefinition of symbol \'" + symbol + "\'");
}
