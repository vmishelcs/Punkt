#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <unordered_map>
#include <string>

struct SymbolMetadata {
    bool is_mutable;
};

class SymbolTable {
public:
    SymbolTable() {

    }

private:
    std::unordered_map<std::string, SymbolMetadata> table;
};

#endif // SYMBOL_TABLE_H_
