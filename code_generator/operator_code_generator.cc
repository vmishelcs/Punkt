#include "operator_code_generator.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

llvm::Value *OperatorCodeGenerator::UnaryNoOp(llvm::LLVMContext *context,
                                              llvm::IRBuilder<> *builder,
                                              llvm::Value *operand) {
  return operand;
}

//--------------------------------------------------------------------------------------//
//                                       Booleans //
//--------------------------------------------------------------------------------------//

llvm::Value *OperatorCodeGenerator::BooleanCmpEqCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto lhs_trunc =
      builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*context), "trunctmp");
  auto lhs_zext = builder->CreateZExt(
      lhs_trunc, llvm::Type::getInt32Ty(*context), "zexttmp");

  auto rhs_trunc =
      builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*context), "trunctmp");
  auto rhs_zext = builder->CreateZExt(
      rhs_trunc, llvm::Type::getInt32Ty(*context), "zexttmp");

  auto i1_result = builder->CreateICmpEQ(lhs_zext, rhs_zext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::BooleanCmpNeqCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto lhs_trunc =
      builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*context), "trunctmp");
  auto lhs_zext = builder->CreateZExt(
      lhs_trunc, llvm::Type::getInt32Ty(*context), "zexttmp");

  auto rhs_trunc =
      builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*context), "trunctmp");
  auto rhs_zext = builder->CreateZExt(
      rhs_trunc, llvm::Type::getInt32Ty(*context), "zexttmp");

  auto i1_result = builder->CreateICmpNE(lhs_zext, rhs_zext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

//--------------------------------------------------------------------------------------//
//                                      Characters //
//--------------------------------------------------------------------------------------//
llvm::Value *OperatorCodeGenerator::CharacterCmpEQCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpEQ(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::CharacterCmpNEQCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpNE(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::CharacterCmpGTCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSGT(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::CharacterCmpLTCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSLT(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::CharacterCmpGEQCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSGE(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::CharacterCmpLEQCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSLE(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

//--------------------------------------------------------------------------------------//
//                                       Integers //
//--------------------------------------------------------------------------------------//
llvm::Value *OperatorCodeGenerator::IntegerNegationCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder,
    llvm::Value *operand) {
  auto neg_val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), -1);
  return builder->CreateMul(operand, neg_val, "negtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerAddCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  return builder->CreateAdd(lhs, rhs, "addtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerSubtractCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  return builder->CreateSub(lhs, rhs, "subtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerMultiplyCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  return builder->CreateMul(lhs, rhs, "multmp");
}

llvm::Value *OperatorCodeGenerator::IntegerDivideCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  return builder->CreateSDiv(lhs, rhs, "divtmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpEQCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpEQ(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpNEQCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpNE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpGTCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpSGT(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpLTCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpSLT(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpGEQCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpSGE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *OperatorCodeGenerator::IntegerCmpLEQCodeGenerator(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpSLE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}
