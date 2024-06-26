#ifndef IDENTIFIER_NODE_H_
#define IDENTIFIER_NODE_H_

#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <symbol_table/symbol_table.h>
#include <token/token.h>

#include <memory>
#include <string>

class IdentifierNode : public ParseNode {
 public:
  IdentifierNode(std::unique_ptr<Token> token);

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  const std::string &GetName() const { return name; }

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  /// @brief Searches the parse tree symbol tables for the declaration of this
  /// identifier.
  /// @return A pointer to the symbol table entry that declares this
  /// identifier.
  SymbolTableEntry *FindSymbolTableEntry();

  void SetSymbolTableEntry(SymbolTableEntry *symbol_table_entry) {
    this->symbol_table_entry = symbol_table_entry;
  }
  SymbolTableEntry *GetSymbolTableEntry() const { return symbol_table_entry; }

  /// @brief Checks if this node is a target of an assignment operation.
  /// @return `true` if this identifer is a target of an assignment operation,
  /// `false` otherwise.
  bool IsAssignmentTarget() const;

  void SetLLVMAlloca(llvm::AllocaInst *alloca);
  void SetLLVMFunction(llvm::Function *function);

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  std::string name;
  SymbolTableEntry *symbol_table_entry;
};

#endif  // IDENTIFIER_NODE_H_
