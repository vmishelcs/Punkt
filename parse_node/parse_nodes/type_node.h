#ifndef TYPE_NODE_H_
#define TYPE_NODE_H_

#include <string>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

#include <parse_node/parse_node.h>
#include <token/token.h>

class TypeNode : public ParseNode {
public:
    TypeNode(std::unique_ptr<Token> token);

    /// @brief Set `Type` based on the token used to create this `TypeNode`.
    void InferOwnType();

    bool DenotesParameterType() const {
        return GetParent()->GetParseNodeType() == ParseNodeType::LAMBDA_PARAMETER_NODE;
    }

    bool DenotesReturnType() const {
        return GetParent()->GetParseNodeType() == ParseNodeType::LAMBDA_NODE;
    }
    
    llvm::Type *GetLLVMType(llvm::LLVMContext& context) const;

    virtual std::string ToString() const override { return "TYPE NODE"; }

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // TYPE_NODE_H_
