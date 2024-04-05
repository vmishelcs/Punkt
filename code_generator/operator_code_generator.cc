#include "operator_code_generator.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

llvm::Value *OperatorCodeGenerator::UnaryNoOp(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *operand) {
    return operand;
}

//--------------------------------------------------------------------------------------//
//                                       Booleans                                       //
//--------------------------------------------------------------------------------------//

llvm::Value *OperatorCodeGenerator::BooleanCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    auto lhs_trunc = builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*context), "trunctmp");
    auto lhs_zext = builder->CreateZExt(lhs_trunc, llvm::Type::getInt32Ty(*context), "zexttmp");

    auto rhs_trunc = builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*context), "trunctmp");
    auto rhs_zext = builder->CreateZExt(rhs_trunc, llvm::Type::getInt32Ty(*context), "zexttmp");

    return builder->CreateICmpEQ(lhs_zext, rhs_zext, "cmptmp");
}

llvm::Value *OperatorCodeGenerator::BooleanCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    auto lhs_trunc = builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*context), "trunctmp");
    auto lhs_zext = builder->CreateZExt(lhs_trunc, llvm::Type::getInt32Ty(*context), "zexttmp");

    auto rhs_trunc = builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*context), "trunctmp");
    auto rhs_zext = builder->CreateZExt(rhs_trunc, llvm::Type::getInt32Ty(*context), "zexttmp");

    return builder->CreateICmpNE(lhs_zext, rhs_zext, "cmptmp");
}

//--------------------------------------------------------------------------------------//
//                                      Characters                                      //
//--------------------------------------------------------------------------------------//
llvm::Value *OperatorCodeGenerator::CharacterCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    auto lhs_sext = builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
    auto rhs_sext = builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");
    return builder->CreateICmpEQ(lhs_sext, rhs_sext, "cmptmp");
}

llvm::Value *OperatorCodeGenerator::CharacterCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    auto lhs_sext = builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
    auto rhs_sext = builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");
    return builder->CreateICmpNE(lhs_sext, rhs_sext, "cmptmp");
}

//--------------------------------------------------------------------------------------//
//                                       Integers                                       //
//--------------------------------------------------------------------------------------//
llvm::Value *OperatorCodeGenerator::IntegerNegationCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *operand) {
    auto neg_val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), -1);
    return builder->CreateMul(operand, neg_val, "negtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerAddCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateAdd(lhs, rhs, "addtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerSubtractCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateSub(lhs, rhs, "subtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerMultiplyCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateMul(lhs, rhs, "multmp");
}

llvm::Value *OperatorCodeGenerator::IntegerDivideCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateSDiv(lhs, rhs, "divtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateICmpEQ(lhs, rhs, "cmptmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateICmpNE(lhs, rhs, "cmptmp");
}
