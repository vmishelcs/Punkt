#include "operator_codegen.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

/******************************************************************************
 *                                 Assignment                                 *
 ******************************************************************************/
llvm::Value *operator_codegen::AssignmentCodegen(llvm::LLVMContext *context,
                                                 llvm::IRBuilder<> *builder,
                                                 llvm::Value *target,
                                                 llvm::Value *new_value) {
  builder->CreateStore(new_value, target);
  return new_value;
}

/******************************************************************************
 *                                    NOP                                     *
 ******************************************************************************/
llvm::Value *operator_codegen::UnaryNop(llvm::LLVMContext *context,
                                        llvm::IRBuilder<> *builder,
                                        llvm::Value *operand) {
  return operand;
}

/******************************************************************************
 *                                  Booleans                                  *
 ******************************************************************************/
llvm::Value *operator_codegen::BooleanNegationCodegen(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder,
    llvm::Value *operand) {
  auto operand_trunc = builder->CreateTrunc(
      operand, llvm::Type::getInt1Ty(*context), "trunctmp");

  // XOR the operand with 1.
  auto i1_result = builder->CreateXor(
      operand_trunc,
      llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), true), "xortmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::BooleanCmpEQCodegen(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
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

llvm::Value *operator_codegen::BooleanCmpNEQCodegen(llvm::LLVMContext *context,
                                                    llvm::IRBuilder<> *builder,
                                                    llvm::Value *lhs,
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

llvm::Value *operator_codegen::BooleanAndCodegen(llvm::LLVMContext *context,
                                                 llvm::IRBuilder<> *builder,
                                                 llvm::Value *lhs,
                                                 llvm::Value *rhs) {
  auto lhs_trunc =
      builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*context), "trunctmp");
  auto rhs_trunc =
      builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*context), "trunctmp");

  auto i1_result = builder->CreateAnd(lhs_trunc, rhs_trunc, "andtmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::BooleanOrCodegen(llvm::LLVMContext *context,
                                                llvm::IRBuilder<> *builder,
                                                llvm::Value *lhs,
                                                llvm::Value *rhs) {
  auto lhs_trunc =
      builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*context), "trunctmp");
  auto rhs_trunc =
      builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*context), "trunctmp");

  auto i1_result = builder->CreateOr(lhs_trunc, rhs_trunc, "ortmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

/******************************************************************************
 *                                 Characters                                 *
 ******************************************************************************/
llvm::Value *operator_codegen::CharacterCmpEQCodegen(llvm::LLVMContext *context,
                                                     llvm::IRBuilder<> *builder,
                                                     llvm::Value *lhs,
                                                     llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpEQ(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpNEQCodegen(
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

llvm::Value *operator_codegen::CharacterCmpGTCodegen(llvm::LLVMContext *context,
                                                     llvm::IRBuilder<> *builder,
                                                     llvm::Value *lhs,
                                                     llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSGT(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpLTCodegen(llvm::LLVMContext *context,
                                                     llvm::IRBuilder<> *builder,
                                                     llvm::Value *lhs,
                                                     llvm::Value *rhs) {
  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSLT(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpGEQCodegen(
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

llvm::Value *operator_codegen::CharacterCmpLEQCodegen(
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

/******************************************************************************
 *                                  Integers                                  *
 ******************************************************************************/
llvm::Value *operator_codegen::IntegerNegationCodegen(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder,
    llvm::Value *operand) {
  auto neg_val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), -1);
  return builder->CreateMul(operand, neg_val, "negtmp");
}

llvm::Value *operator_codegen::IntegerAddCodegen(llvm::LLVMContext *context,
                                                 llvm::IRBuilder<> *builder,
                                                 llvm::Value *lhs,
                                                 llvm::Value *rhs) {
  return builder->CreateAdd(lhs, rhs, "addtmp");
}

llvm::Value *operator_codegen::IntegerSubtractCodegen(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  return builder->CreateSub(lhs, rhs, "subtmp");
}

llvm::Value *operator_codegen::IntegerMultiplyCodegen(
    llvm::LLVMContext *context, llvm::IRBuilder<> *builder, llvm::Value *lhs,
    llvm::Value *rhs) {
  return builder->CreateMul(lhs, rhs, "multmp");
}

llvm::Value *operator_codegen::IntegerDivideCodegen(llvm::LLVMContext *context,
                                                    llvm::IRBuilder<> *builder,
                                                    llvm::Value *lhs,
                                                    llvm::Value *rhs) {
  return builder->CreateSDiv(lhs, rhs, "divtmp");
}

llvm::Value *operator_codegen::IntegerModuloCodegen(llvm::LLVMContext *context,
                                                    llvm::IRBuilder<> *builder,
                                                    llvm::Value *lhs,
                                                    llvm::Value *rhs) {
  return builder->CreateSRem(lhs, rhs, "modtmp");
}

llvm::Value *operator_codegen::IntegerCmpEQCodegen(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
                                                   llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpEQ(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpNEQCodegen(llvm::LLVMContext *context,
                                                    llvm::IRBuilder<> *builder,
                                                    llvm::Value *lhs,
                                                    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpNE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpGTCodegen(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
                                                   llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpSGT(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpLTCodegen(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
                                                   llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpSLT(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpGEQCodegen(llvm::LLVMContext *context,
                                                    llvm::IRBuilder<> *builder,
                                                    llvm::Value *lhs,
                                                    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpSGE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpLEQCodegen(llvm::LLVMContext *context,
                                                    llvm::IRBuilder<> *builder,
                                                    llvm::Value *lhs,
                                                    llvm::Value *rhs) {
  auto i1_result = builder->CreateICmpSLE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

/******************************************************************************
 *                                   Arrays                                   *
 ******************************************************************************/
llvm::Value *operator_codegen::AllocCodegen(llvm::LLVMContext *context,
                                            llvm::IRBuilder<> *builder,
                                            llvm::Value *elem_size,
                                            llvm::Value *arr_size) {
  return nullptr;
}

llvm::Value *operator_codegen::IndexingCodegen(llvm::LLVMContext *context,
                                               llvm::IRBuilder<> *builder,
                                               llvm::Value *arr,
                                               llvm::Value *idx) {
  return nullptr;
}
