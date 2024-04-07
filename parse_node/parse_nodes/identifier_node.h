#ifndef IDENTIFIER_NODE_H_
#define IDENTIFIER_NODE_H_

#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>

#include <parse_node/parse_node.h>
#include <symbol_table/symbol_table.h>

class IdentifierNode : public ParseNode {
public:
    IdentifierNode(std::unique_ptr<Token> token);

    std::string GetName() const;

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    std::optional<std::reference_wrapper<SymbolTableEntry>> FindSymbolTableEntry();

    Type *FindType();
    llvm::AllocaInst *FindAlloca();

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // IDENTIFIER_NODE_H_
