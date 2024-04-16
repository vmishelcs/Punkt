#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>

#include <input_handler/text_location.h>
#include <semantic_analyzer/types/type.h>

enum class SymbolType {
    VARIABLE,
    LAMBDA
};

struct SymbolTableEntry {
    const TextLocation text_location;
    bool is_mutable;
    Type *type;
    SymbolType symbol_type;
    llvm::AllocaInst *alloca;
    llvm::Function *function;
};

class SymbolTable {
public:
    /// @brief Insert a variable into the symbol table.
    /// @param symbol Identifier name representing the symbol.
    /// @param tl Reference to a `TextLocation` object of the symbol.
    /// @param is_mutable Determines whether the variable's value represented by the symbol can be
    /// changed.
    /// @param type Variable type.
    /// @param symbol_type The type of symbol table entry (variable or function).
    /// @returns A pointer to the newly inserted symbol table entry.
    SymbolTableEntry *Insert(const std::string& symbol,
            const TextLocation& tl,
            bool is_mutable,
            Type *type,
            SymbolType symbol_type);

    SymbolTableEntry *Get(const std::string& symbol);

    bool Contains(const std::string& symbol) const;

    static void UndefinedSymbolReference(const std::string& symbol, const TextLocation& tl);

private:
    void SymbolRedefinitionError(const std::string& symbol, const TextLocation& tl);

    std::unordered_map<std::string, std::unique_ptr<SymbolTableEntry>> table;
};

#endif // SYMBOL_TABLE_H_
