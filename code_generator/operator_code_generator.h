#ifndef OPERATOR_CODE_GENERATOR_H_
#define OPERATOR_CODE_GENERATOR_H_

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

class OperatorCodeGenerator
{
public:
    static llvm::Value *UnaryNoOp(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *operand);

    // ---- Booleans -------------------------------------------------------------------------
    static llvm::Value *BooleanCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);
    static llvm::Value *BooleanCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);

    // ---- Characters -----------------------------------------------------------------------
    static llvm::Value *CharacterCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);
    static llvm::Value *CharacterCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);

    // ---- Integers -------------------------------------------------------------------------
    static llvm::Value *IntegerNegationCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *operand);
    static llvm::Value *IntegerAddCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);
    static llvm::Value *IntegerSubtractCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);
    static llvm::Value *IntegerMultiplyCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);
    static llvm::Value *IntegerDivideCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);
    static llvm::Value *IntegerCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);
    static llvm::Value *IntegerCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs);
};

#endif // OPERATOR_CODE_GENERATOR_H_
