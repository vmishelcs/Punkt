#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <unordered_map>
#include <string>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>

#include <input_handler/text_location.h>
#include <semantic_analyzer/type.h>

struct SymbolData {
    const TextLocation& text_location;
    bool is_mutable;
    const Type& type;
    llvm::AllocaInst *binding;
};

class SymbolTable {
public:
    void Insert(const std::string& symbol, const TextLocation& tl, bool is_mutable, const Type& type);
    SymbolData& Get(const std::string& symbol);

    bool Contains(const std::string& symbol) const;

    static void UndefinedSymbolReference(const std::string& symbol, const TextLocation& tl);

private:
    void SymbolRedefinitionError(const std::string& symbol, const TextLocation& tl);

    std::unordered_map<std::string, SymbolData> table;
};

#endif // SYMBOL_TABLE_H_
