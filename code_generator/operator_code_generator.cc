#include "operator_code_generator.h"

llvm::Value *OperatorCodeGenerator::UnaryNoOp(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *operand) {
    return operand;
}

//--------------------------------------------------------------------------------------//
//                                       Booleans                                       //
//--------------------------------------------------------------------------------------//

llvm::Value *OperatorCodeGenerator::BooleanCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateICmpEQ(lhs, rhs, "bool_cmpeqtmp");
}

llvm::Value *OperatorCodeGenerator::BooleanCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateICmpNE(lhs, rhs, "bool_cmpneqtmp");
}

//--------------------------------------------------------------------------------------//
//                                      Characters                                      //
//--------------------------------------------------------------------------------------//
llvm::Value *OperatorCodeGenerator::CharacterCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateICmpEQ(lhs, rhs, "char_cmpeqtmp");
}

llvm::Value *OperatorCodeGenerator::CharacterCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateICmpNE(lhs, rhs, "char_cmpneqtmp");
}

//--------------------------------------------------------------------------------------//
//                                       Integers                                       //
//--------------------------------------------------------------------------------------//
llvm::Value *OperatorCodeGenerator::IntegerNegationCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *operand) {
    auto val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), -1);
    return builder->CreateMul(operand, val, "int_negtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerAddCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateAdd(lhs, rhs, "int_addtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerSubtractCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateSub(lhs, rhs, "int_subtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerMultiplyCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateMul(lhs, rhs, "int_multmp");
}

llvm::Value *OperatorCodeGenerator::IntegerDivideCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateSDiv(lhs, rhs, "int_divtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpEqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateICmpEQ(lhs, rhs, "int_cmpeqtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpNeqCodeGenerator(llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs, llvm::Value *rhs)
{
    return builder->CreateICmpNE(lhs, rhs, "int_cmpneqtmp");
}
