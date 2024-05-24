#include "operator_codegen.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <semantic_analyzer/types/array_type.h>
#include <semantic_analyzer/types/type.h>

#include "code_generation_visitor.h"
#include "codegen_context.h"

/// @brief Simplify the specified rational number such that the numerator and
/// denominator are co-prime. Additionally, make sure that if the rational
/// number is positive, it has the form `a/b`. If it is negative, it has the
/// form `-a/b`.
/// @param numerator Numerator of the rational number specified as a 64-bit
/// integer.
/// @param denominator Denominator of the rational number specified as a 64-bit
/// integer.
static void SimplifyRational(llvm::Value *&numerator,
                             llvm::Value *&denominator);

/// @brief Generate LLVM IR that computes the GCD of the provided arguments.
/// @param a `llvm::Value` pointer to a 64-bit integer.
/// @param b `llvm::Value` pointer to a 64-bit integer.
/// @return `llvm::Value` pointer to a 64-bit integer representing the GCD of
/// `a` and `b`.
static llvm::Value *GenerateGCD(llvm::Value *a, llvm::Value *b);

//===----------------------------------------------------------------------===//
// Assignment
//===----------------------------------------------------------------------===//
llvm::Value *operator_codegen::AssignmentCodegen(CodeGenerationVisitor &cv,
                                                 OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *target = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *new_value = node.GetChild(1)->GenerateCode(cv);

  Type *new_value_type = node.GetChild(1)->GetType();
  if (!(dynamic_cast<BaseType *>(new_value_type)
            ->IsEquivalentTo(BaseTypeEnum::RATIONAL))) {
    // If the new value does not have rational type, just store it and return.
    builder->CreateStore(new_value, target);
    return new_value;
  }

  // Otherwise, we need to simplify the rational number.
  llvm::Value *num = builder->CreateShl(new_value, 64, "lhsnum");
  num = builder->CreateLShr(num, 64, "lhsnum");
  num = builder->CreateTrunc(num, llvm::Type::getInt64Ty(*llvm_context),
                             "trunctmp");
  llvm::Value *denom = builder->CreateLShr(new_value, 64, "lhsdenom");
  denom = builder->CreateTrunc(denom, llvm::Type::getInt64Ty(*llvm_context),
                               "trunctmp");

  // Compute GCD of the numerator and denominator.
  llvm::Value *gcd = GenerateGCD(num, denom);

  num = builder->CreateSDiv(num, gcd, "divtmp");
  num = builder->CreateZExt(num, llvm::Type::getInt128Ty(*llvm_context),
                            "zexttmp");
  denom = builder->CreateSDiv(denom, gcd, "divtmp");
  denom = builder->CreateZExt(denom, llvm::Type::getInt128Ty(*llvm_context),
                              "zexttmp");
  denom = builder->CreateShl(denom, 64, "shltmp");
  return builder->CreateOr(num, denom, "ortmp");
}

//===----------------------------------------------------------------------===//
// NOP
//===----------------------------------------------------------------------===//
llvm::Value *operator_codegen::UnaryNop(CodeGenerationVisitor &cv,
                                        OperatorNode &node) {
  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);
  return operand;
}

/******************************************************************************
 *                                  Booleans                                  *
 ******************************************************************************/
llvm::Value *operator_codegen::BooleanNegationCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);

  auto operand_trunc = builder->CreateTrunc(
      operand, llvm::Type::getInt1Ty(*context), "trunctmp");

  // XOR the operand with 1.
  auto i1_result = builder->CreateXor(
      operand_trunc,
      llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), true), "xortmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::BooleanCmpEQCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

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

llvm::Value *operator_codegen::BooleanCmpNEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

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

llvm::Value *operator_codegen::BooleanAndCodegen(CodeGenerationVisitor &cv,
                                                 OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto lhs_trunc =
      builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*context), "trunctmp");
  auto rhs_trunc =
      builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*context), "trunctmp");

  auto i1_result = builder->CreateAnd(lhs_trunc, rhs_trunc, "andtmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::BooleanOrCodegen(CodeGenerationVisitor &cv,
                                                OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

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
llvm::Value *operator_codegen::CharacterCmpEQCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpEQ(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpNEQCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpNE(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpGTCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSGT(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpLTCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSLT(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpGEQCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSGE(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpLEQCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

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
llvm::Value *operator_codegen::IntegerNegationCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);

  return builder->CreateSub(
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0), operand,
      "negtmp");
}

llvm::Value *operator_codegen::IntegerAddCodegen(CodeGenerationVisitor &cv,
                                                 OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateAdd(lhs, rhs, "addtmp");
}

llvm::Value *operator_codegen::IntegerSubtractCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateSub(lhs, rhs, "subtmp");
}

llvm::Value *operator_codegen::IntegerMultiplyCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateMul(lhs, rhs, "multmp");
}

llvm::Value *operator_codegen::IntegerDivideCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateSDiv(lhs, rhs, "divtmp");
}

llvm::Value *operator_codegen::IntegerModuloCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateSRem(lhs, rhs, "modtmp");
}

llvm::Value *operator_codegen::IntegerCmpEQCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpEQ(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpNEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpNE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpGTCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpSGT(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpLTCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpSLT(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpGEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpSGE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpLEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpSLE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

//===----------------------------------------------------------------------===//
// Rationals
//===----------------------------------------------------------------------===//
llvm::Value *operator_codegen::OverOperatorCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  // Rational numbers are stored in a 128-bit integer. First 64-bits are
  // represent the numerator, last 64-bits represent the denominator.
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *num = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *denom = node.GetChild(1)->GenerateCode(cv);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (!next_op || !next_op->IsArithmeticOperation()) {
    // If the parent is not another arithmetic operation, simplify the rational
    // number now.
    SimplifyRational(num, denom);
  }

  // Zero-extend numerator and denominator to 128-bit integers.
  num = builder->CreateZExt(num, llvm::Type::getInt128Ty(*llvm_context));
  denom = builder->CreateZExt(denom, llvm::Type::getInt128Ty(*llvm_context));
  denom = builder->CreateShl(denom, 64);

  llvm::Value *result = builder->CreateOr(num, denom);

  return result;
}

llvm::Value *operator_codegen::RationalAddCodegen(CodeGenerationVisitor &cv,
                                                  OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);
  llvm::Type *int128_type = llvm::Type::getInt128Ty(*llvm_context);

  // We need to extract the numerator and denominator out of the left-hand side
  // as 64-bit integers.
  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *lhs_num = builder->CreateShl(lhs, 64);
  lhs_num = builder->CreateLShr(lhs_num, 64);
  lhs_num = builder->CreateTrunc(lhs_num, int64_type);
  llvm::Value *lhs_denom = builder->CreateLShr(lhs, 64);
  lhs_denom = builder->CreateTrunc(lhs_denom, int64_type);

  // Now extract the numerator and denominator out of the right-hand side as
  // 64-bit integers.
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  llvm::Value *rhs_num = builder->CreateShl(rhs, 64);
  rhs_num = builder->CreateLShr(rhs_num, 64);
  rhs_num = builder->CreateTrunc(rhs_num, int64_type);
  llvm::Value *rhs_denom = builder->CreateLShr(rhs, 64);
  rhs_denom = builder->CreateTrunc(rhs_denom, int64_type);

  // a/b + c/d = (a*d + c*b) / b*d
  // Multiply LHS numerator by RHS denominator.
  lhs_num = builder->CreateMul(lhs_num, rhs_denom);

  // Multiply RHS numerator by LHS denominator.
  rhs_num = builder->CreateMul(rhs_num, lhs_denom);

  llvm::Value *result_num = builder->CreateAdd(lhs_num, rhs_num);
  llvm::Value *result_denom = builder->CreateMul(lhs_denom, rhs_denom);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (next_op && !next_op->IsArithmeticOperation()) {
    SimplifyRational(result_num, result_denom);
  }

  result_num = builder->CreateZExt(result_num, int128_type);
  result_denom = builder->CreateZExt(result_denom, int128_type);

  // Shift denominator left by 64 bits in preparation for storage.
  result_denom = builder->CreateShl(result_denom, 64);

  llvm::Value *result = builder->CreateOr(result_num, result_denom);

  return result;
}

llvm::Value *operator_codegen::RationalSubtractCodegen(
    CodeGenerationVisitor &cv, OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  // We need to extract the numerator and denominator out of the left-hand side.
  llvm::Value *lhs_num = builder->CreateShl(lhs, 64);
  lhs_num = builder->CreateLShr(lhs_num, 64);
  llvm::Value *lhs_denom = builder->CreateLShr(lhs, 64);

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  // Now extract the numerator and denominator out of the right-hand side.
  llvm::Value *rhs_num = builder->CreateShl(rhs, 64);
  rhs_num = builder->CreateLShr(rhs_num, 64);
  llvm::Value *rhs_denom = builder->CreateLShr(rhs, 64);

  // a/b - c/d = (a*d - c*b) / b*d
  // Multiply LHS numerator by RHS denominator.
  lhs_num = builder->CreateMul(lhs_num, rhs_denom);

  // Multiply RHS numerator by LHS denominator.
  rhs_num = builder->CreateMul(rhs_num, lhs_denom);

  llvm::Value *result_num = builder->CreateSub(lhs_num, rhs_num);
  llvm::Value *result_denom = builder->CreateMul(lhs_denom, rhs_denom);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (next_op && !next_op->IsArithmeticOperation()) {
    SimplifyRational(result_num, result_denom);
  }

  // Shift denominator left by 64 bits in preparation for storage.
  result_denom = builder->CreateShl(result_denom, 64);

  llvm::Value *result = builder->CreateOr(result_num, result_denom);

  return result;
}

llvm::Value *operator_codegen::RationalMultiplyCodegen(
    CodeGenerationVisitor &cv, OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  // We need to extract the numerator and denominator out of the left-hand side.
  llvm::Value *lhs_num = builder->CreateShl(lhs, 64);
  lhs_num = builder->CreateLShr(lhs_num, 64);
  llvm::Value *lhs_denom = builder->CreateLShr(lhs, 64);

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  // Now extract the numerator and denominator out of the right-hand side.
  llvm::Value *rhs_num = builder->CreateShl(rhs, 64);
  rhs_num = builder->CreateLShr(rhs_num, 64);
  llvm::Value *rhs_denom = builder->CreateLShr(rhs, 64);

  // Multiply numerators and denominators.
  llvm::Value *result_num = builder->CreateMul(lhs_num, rhs_num);
  llvm::Value *result_denom = builder->CreateMul(lhs_denom, rhs_denom);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (next_op && !next_op->IsArithmeticOperation()) {
    SimplifyRational(result_num, result_denom);
  }

  // Shift denominator left by 64 bits in preparation for storage.
  result_denom = builder->CreateShl(result_denom, 64);

  llvm::Value *result = builder->CreateOr(result_num, result_denom);

  return result;
}

llvm::Value *operator_codegen::RationalDivideCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  // We need to extract the numerator and denominator out of the left-hand side.
  llvm::Value *lhs_num = builder->CreateShl(lhs, 64);
  lhs_num = builder->CreateLShr(lhs_num, 64);
  llvm::Value *lhs_denom = builder->CreateLShr(lhs, 64);

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  // Now extract the numerator and denominator out of the right-hand side.
  llvm::Value *rhs_num = builder->CreateShl(rhs, 64);
  rhs_num = builder->CreateLShr(rhs_num, 64);
  llvm::Value *rhs_denom = builder->CreateLShr(rhs, 64);

  // Perform rational number division by computing
  // (LHS_numerator * RHS_denominator) / (LHS_denominator * RHS_numerator).
  llvm::Value *result_num = builder->CreateMul(lhs_num, rhs_denom);
  llvm::Value *result_denom = builder->CreateMul(lhs_denom, rhs_num);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (next_op && !next_op->IsArithmeticOperation()) {
    SimplifyRational(result_num, result_denom);
  }

  // Shift denominator left by 64 bits in preparation for storage.
  result_denom = builder->CreateShl(result_denom, 64);

  llvm::Value *result = builder->CreateOr(result_num, result_denom);

  return result;
}

llvm::Value *operator_codegen::RationalCmpEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  return builder->CreateICmpEQ(lhs, rhs, "cmptmp");
}

/******************************************************************************
 *                                   Arrays *
 ******************************************************************************/
llvm::Value *operator_codegen::ArrayIndexingCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Find Punkt array struct type.
  const std::string &PunktArray_struct_name = cv.GetPunktArrayStructName();
  llvm::StructType *PunktArray_struct =
      llvm::StructType::getTypeByName(*llvm_context, PunktArray_struct_name);

  // Load the data portion of the Punkt array object.
  llvm::Value *PunktArray_ptr = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *PunktArray_data_ptr = builder->CreateGEP(
      PunktArray_struct, PunktArray_ptr,
      {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 0),
       llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 1)},
      "PunktArray_data_ptr");
  llvm::Value *PunktArray_data =
      builder->CreateLoad(llvm::PointerType::getUnqual(*llvm_context),
                          PunktArray_data_ptr, "PunktArray_data");

  ArrayType *array_type = static_cast<ArrayType *>(node.GetChild(0)->GetType());
  Type *subtype = array_type->GetSubtype();
  llvm::Type *llvm_subtype = subtype->GetLLVMType(*llvm_context);
  llvm::Value *idx = node.GetChild(1)->GenerateCode(cv);

  // Issue a runtime error if index is negative.
  llvm::Function *parent_function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *negative_index_true = llvm::BasicBlock::Create(
      *llvm_context, "negative_index_true", parent_function);
  llvm::BasicBlock *negative_index_false = llvm::BasicBlock::Create(
      *llvm_context, "negative_index_false", parent_function);
  llvm::Value *negative_index_check = builder->CreateICmpSLT(
      idx, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 0),
      "negative_index_check");
  builder->CreateCondBr(negative_index_check, negative_index_true,
                        negative_index_false);

  builder->SetInsertPoint(negative_index_true);
  cv.GenerateRuntimeErrorWithMessage("negative array index");

  builder->SetInsertPoint(negative_index_false);

  // Check if index is greater than or equal to array size.
  llvm::Value *arr_size = builder->CreateLoad(
      llvm::Type::getInt64Ty(*llvm_context), PunktArray_ptr, "arr_size");
  llvm::BasicBlock *array_ooo_true = llvm::BasicBlock::Create(
      *llvm_context, "array_ooo_true", parent_function);
  llvm::BasicBlock *array_ooo_false = llvm::BasicBlock::Create(
      *llvm_context, "array_ooo_false", parent_function);
  llvm::Value *array_ooo_check =
      builder->CreateICmpUGE(idx, arr_size, "array_ooo_check");
  builder->CreateCondBr(array_ooo_check, array_ooo_true, array_ooo_false);

  builder->SetInsertPoint(array_ooo_true);
  cv.GenerateRuntimeErrorWithMessage("array index out of bounds");

  builder->SetInsertPoint(array_ooo_false);

  llvm::Value *elem_addr =
      builder->CreateGEP(llvm_subtype, PunktArray_data, {idx}, "elemaddr");

  // If this node is an assignment operation target, we just have to return
  // the address of the indexed element.
  if (node.IsAssignmentTarget()) {
    return elem_addr;
  }

  return builder->CreateLoad(llvm_subtype, elem_addr, "elemval");
}

llvm::Value *operator_codegen::ArraySizeofCodegen(CodeGenerationVisitor &cv,
                                                  OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Size of array is the first field of the PunktArray struct.
  llvm::Value *PunktArray_ptr = node.GetChild(0)->GenerateCode(cv);
  return builder->CreateLoad(llvm::Type::getInt64Ty(*context), PunktArray_ptr,
                             "PunktArray_size");
}

//===----------------------------------------------------------------------===//
// Helper functions
//===----------------------------------------------------------------------===//
void SimplifyRational(llvm::Value *&numerator, llvm::Value *&denominator) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::Module *module = codegen_context->GetModule();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int1_type = llvm::Type::getInt1Ty(*llvm_context);
  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);
  llvm::Value *int64_zero = llvm::ConstantInt::get(int64_type, 0);
  llvm::Value *i1_zero = llvm::ConstantInt::get(int1_type, 0);

  // Format rational number:
  //    a/b  ->  a/b (no change needed in this case)
  //   -a/-b ->  a/b
  //   -a/b  -> -a/b (no change needed in this case)
  //    a/-b -> -a/b

  // Check denominator sign.
  llvm::Value *denom_negative_check =
      builder->CreateICmpSLT(denominator, int64_zero, "cmptmp");

  // Create a value holding the numerator with flipped sign.
  llvm::Value *numerator_flipped_sign =
      builder->CreateSub(int64_zero, numerator, "subtmp");

  // Select instruction to flip the sign of the denominator.
  numerator = builder->CreateSelect(denom_negative_check,
                                    numerator_flipped_sign, numerator);

  // Set denominator to abs(denominator).
  llvm::Function *abs_intrinsic = llvm::Intrinsic::getDeclaration(
      module, llvm::Intrinsic::abs, {int64_type});
  denominator =
      builder->CreateCall(abs_intrinsic, {denominator, i1_zero}, "calltmp");

  // Get GCD(numerator, denominator). Use abs(numerator) to compute the GCD.
  llvm::Value *abs_numerator =
      builder->CreateCall(abs_intrinsic, {numerator, i1_zero}, "calltmp");
  llvm::Value *gcd = GenerateGCD(abs_numerator, denominator);

  // Divide numerator and denominator by GCD.
  numerator = builder->CreateSDiv(numerator, gcd, "divtmp");
  denominator = builder->CreateSDiv(denominator, gcd, "divtmp");
}

llvm::Value *GenerateGCD(llvm::Value *a, llvm::Value *b) {
  // The LLVM IR in this function implements the GCD algorithm below.
  // gcd(a, b):
  //   while (a != b):
  //     if (a > b):
  //       a = a - b
  //     else:
  //       b = b - a
  //   return a
  //
  // GCD algorithm LLVM IR outline:
  //
  // gcd_before:
  //   ...
  //   %gcdarg1 = add 0, arg1
  //   %gcdarg2 = add 0, arg2
  //   br label %gcd_loop_cond
  //
  // gcd_loop_cond:
  //   %gcda = [%gcdarg1, %gcd_before], [%nexta, %gcd_if_merge]
  //   %gcdb = [%gcdarg2, %gcd_before], [%nextb, %gcd_if_merge]
  //   %condcheck = icmp ne %gcda, %gcdb
  //   %br %condcheck, label %gcd_loop_body, %gcd_loop_end
  //
  // gcd_loop_body:
  //   %ifcheck = %icmp sgt %gcda, %gcdb
  //   br %ifcheck, label %gcd_then, %gcd_else
  //
  // gcd_then:
  //   %reducea = sub %gcda, %gcdb
  //   br label %gcd_if_merge
  //
  // gcd_else:
  //   %reduceb = sub %gcdb, %gcda
  //   br label %gcd_if_merge
  //
  // gcd_if_merge:
  //   %nexta = phi [%reducea, %gcd_then], [%gcda, %gcd_else]
  //   %nextb = phi [%reduceb, %gcd_else], [%gcdb, %gcd_then]
  //   br label %gcd_loop_cond
  //
  // gcd_loop_end:
  //   ; Here, `gcda` is the GCD.

  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Create necessary basic blocks.
  llvm::Function *parent_function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *gcd_before = builder->GetInsertBlock();
  llvm::BasicBlock *gcd_loop_cond =
      llvm::BasicBlock::Create(*llvm_context, "gcd_loop_cond");
  llvm::BasicBlock *gcd_loop_body =
      llvm::BasicBlock::Create(*llvm_context, "gcd_loop_body");
  llvm::BasicBlock *gcd_then =
      llvm::BasicBlock::Create(*llvm_context, "gcd_then");
  llvm::BasicBlock *gcd_else =
      llvm::BasicBlock::Create(*llvm_context, "gcd_else");
  llvm::BasicBlock *gcd_if_merge =
      llvm::BasicBlock::Create(*llvm_context, "gcd_if_merge");
  llvm::BasicBlock *gcd_loop_end =
      llvm::BasicBlock::Create(*llvm_context, "gcd_loop_end");

  // gcd_before:
  llvm::Value *int64_zero =
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 0);
  llvm::Value *gcdarg1 = builder->CreateAdd(int64_zero, a, "gcdarg1");
  llvm::Value *gcdarg2 = builder->CreateAdd(int64_zero, b, "gcdarg2");
  builder->CreateBr(gcd_loop_cond);

  // gcd_loop_cond:
  parent_function->insert(parent_function->end(), gcd_loop_cond);
  builder->SetInsertPoint(gcd_loop_cond);
  llvm::PHINode *gcda =
      builder->CreatePHI(llvm::Type::getInt64Ty(*llvm_context), 2, "gcda");
  gcda->addIncoming(gcdarg1, gcd_before);
  llvm::PHINode *gcdb =
      builder->CreatePHI(llvm::Type::getInt64Ty(*llvm_context), 2, "gcdb");
  gcdb->addIncoming(gcdarg2, gcd_before);
  llvm::Value *condcheck = builder->CreateICmpNE(gcda, gcdb, "condcheck");
  builder->CreateCondBr(condcheck, gcd_loop_body, gcd_loop_end);

  // gcd_loop_body:
  parent_function->insert(parent_function->end(), gcd_loop_body);
  builder->SetInsertPoint(gcd_loop_body);
  llvm::Value *ifcheck = builder->CreateICmpSGT(gcda, gcdb, "ifcheck");
  builder->CreateCondBr(ifcheck, gcd_then, gcd_else);

  // gcd_then:
  parent_function->insert(parent_function->end(), gcd_then);
  builder->SetInsertPoint(gcd_then);
  llvm::Value *reducea = builder->CreateSub(gcda, gcdb, "reducea");
  builder->CreateBr(gcd_if_merge);

  // gcd_else:
  parent_function->insert(parent_function->end(), gcd_else);
  builder->SetInsertPoint(gcd_else);
  llvm::Value *reduceb = builder->CreateSub(gcdb, gcda, "reduceb");
  builder->CreateBr(gcd_if_merge);

  // gcd_if_merge:
  parent_function->insert(parent_function->end(), gcd_if_merge);
  builder->SetInsertPoint(gcd_if_merge);
  llvm::PHINode *nexta =
      builder->CreatePHI(llvm::Type::getInt64Ty(*llvm_context), 2, "nexta");
  nexta->addIncoming(reducea, gcd_then);
  nexta->addIncoming(gcda, gcd_else);
  gcda->addIncoming(nexta, gcd_if_merge);
  llvm::PHINode *nextb =
      builder->CreatePHI(llvm::Type::getInt64Ty(*llvm_context), 2, "nextb");
  nextb->addIncoming(reduceb, gcd_else);
  nextb->addIncoming(gcdb, gcd_then);
  gcdb->addIncoming(nextb, gcd_if_merge);
  builder->CreateBr(gcd_loop_cond);

  // gcd_loop_end:
  parent_function->insert(parent_function->end(), gcd_loop_end);
  builder->SetInsertPoint(gcd_loop_end);
  return gcda;
}
