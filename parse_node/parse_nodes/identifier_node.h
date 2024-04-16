#ifndef IDENTIFIER_NODE_H_
#define IDENTIFIER_NODE_H_

#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>

#include <parse_node/parse_node.h>
#include <symbol_table/symbol_table.h>
#include <token/token.h>

class IdentifierNode : public ParseNode {
public:
    IdentifierNode(std::unique_ptr<Token> token);

    const std::string& GetName() const { return name; }

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    std::optional<std::reference_wrapper<SymbolTableEntry>> FindSymbolTableEntry();

    void SetLLVMAlloca(llvm::AllocaInst *alloca);
    void SetLLVMFunction(llvm::Function *function);
    llvm::AllocaInst *FindLLVMAlloca();
    llvm::Function *FindLLVMFunction();

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;

private:
    std::string name;
};

#endif // IDENTIFIER_NODE_H_
