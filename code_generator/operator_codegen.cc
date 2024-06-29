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
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *target = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *new_value = node.GetChild(1)->GenerateCode(cv);

  builder->CreateStore(new_value, target);
  return new_value;
}

//===----------------------------------------------------------------------===//
// NOP
//===----------------------------------------------------------------------===//
llvm::Value *operator_codegen::UnaryNop(CodeGenerationVisitor &cv,
                                        OperatorNode &node) {
  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);
  return operand;
}

//===----------------------------------------------------------------------===//
// Booleans
//===----------------------------------------------------------------------===//
llvm::Value *operator_codegen::BooleanNegationCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);
  operand = builder->CreateTrunc(operand, llvm::Type::getInt1Ty(*llvm_context));

  // XOR the operand with 1.
  llvm::Value *i1_true =
      llvm::ConstantInt::get(llvm::Type::getInt1Ty(*llvm_context), true);
  llvm::Value *i1_result = builder->CreateXor(operand, i1_true);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::BooleanCmpEQCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*llvm_context));
  lhs = builder->CreateZExt(lhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*llvm_context));
  rhs = builder->CreateZExt(rhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateICmpEQ(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::BooleanCmpNEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*llvm_context));
  lhs = builder->CreateZExt(lhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*llvm_context));
  rhs = builder->CreateZExt(rhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateICmpNE(lhs, rhs, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::BooleanAndCodegen(CodeGenerationVisitor &cv,
                                                 OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateAnd(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::BooleanOrCodegen(CodeGenerationVisitor &cv,
                                                OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateOr(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

//===----------------------------------------------------------------------===//
// Characters
//===----------------------------------------------------------------------===//
llvm::Value *operator_codegen::CharacterCmpEQCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateICmpEQ(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::CharacterCmpNEQCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateICmpNE(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::CharacterCmpGTCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateICmpSGT(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::CharacterCmpLTCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateICmpSLT(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::CharacterCmpGEQCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateICmpSGE(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::CharacterCmpLEQCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  lhs = builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  rhs = builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*llvm_context));

  llvm::Value *i1_result = builder->CreateICmpSLE(lhs, rhs);

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

//===----------------------------------------------------------------------===//
// Integers
//===----------------------------------------------------------------------===//
llvm::Value *operator_codegen::IntegerNegationCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);

  return builder->CreateSub(
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 0),
      operand);
}

llvm::Value *operator_codegen::IntegerAddCodegen(CodeGenerationVisitor &cv,
                                                 OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateAdd(lhs, rhs);
}

llvm::Value *operator_codegen::IntegerSubtractCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateSub(lhs, rhs);
}

llvm::Value *operator_codegen::IntegerMultiplyCodegen(CodeGenerationVisitor &cv,
                                                      OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateMul(lhs, rhs);
}

llvm::Value *operator_codegen::IntegerDivideCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  // Runtime error for division by 0.
  llvm::Function *parent_function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *zero_div_block = llvm::BasicBlock::Create(*llvm_context);
  llvm::BasicBlock *nonzero_div_block = llvm::BasicBlock::Create(*llvm_context);

  llvm::Value *int64_zero =
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 0);
  llvm::Value *zero_div_cmp = builder->CreateICmpEQ(rhs, int64_zero);
  builder->CreateCondBr(zero_div_cmp, zero_div_block, nonzero_div_block);

  // Zero divisor case.
  parent_function->insert(parent_function->end(), zero_div_block);
  builder->SetInsertPoint(zero_div_block);
  cv.GenerateRuntimeErrorWithMessage("division by 0");

  // We continue as usual for non-zero divisors.
  parent_function->insert(parent_function->end(), nonzero_div_block);
  builder->SetInsertPoint(nonzero_div_block);

  return builder->CreateSDiv(lhs, rhs);
}

llvm::Value *operator_codegen::IntegerModuloCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateSRem(lhs, rhs);
}

llvm::Value *operator_codegen::IntegerCmpEQCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpEQ(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::IntegerCmpNEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpNE(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::IntegerCmpGTCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpSGT(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::IntegerCmpLTCodegen(CodeGenerationVisitor &cv,
                                                   OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpSLT(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::IntegerCmpGEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpSGE(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::IntegerCmpLEQCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateICmpSLE(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

//===----------------------------------------------------------------------===//
// Floating point
//===----------------------------------------------------------------------===//
llvm::Value *operator_codegen::FloatNegationCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);

  return builder->CreateFNeg(operand);
}

llvm::Value *operator_codegen::FloatAddCodegen(CodeGenerationVisitor &cv,
                                               OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateFAdd(lhs, rhs);
}

llvm::Value *operator_codegen::FloatSubtractCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateFSub(lhs, rhs);
}

llvm::Value *operator_codegen::FloatMultiplyCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  return builder->CreateFMul(lhs, rhs);
}

llvm::Value *operator_codegen::FloatDivideCodegen(CodeGenerationVisitor &cv,
                                                  OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  // Runtime error for division by 0.
  llvm::Function *parent_function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *zero_div_block = llvm::BasicBlock::Create(*llvm_context);
  llvm::BasicBlock *nonzero_div_block = llvm::BasicBlock::Create(*llvm_context);

  llvm::Value *float64_zero =
      llvm::ConstantFP::get(*llvm_context, llvm::APFloat(0.0));
  llvm::Value *zero_div_cmp = builder->CreateFCmpOEQ(rhs, float64_zero);
  builder->CreateCondBr(zero_div_cmp, zero_div_block, nonzero_div_block);

  // Zero divisor case.
  parent_function->insert(parent_function->end(), zero_div_block);
  builder->SetInsertPoint(zero_div_block);
  cv.GenerateRuntimeErrorWithMessage("division by 0");

  // We continue as usual for non-zero divisors.
  parent_function->insert(parent_function->end(), nonzero_div_block);
  builder->SetInsertPoint(nonzero_div_block);

  return builder->CreateFDiv(lhs, rhs);
}

llvm::Value *operator_codegen::FloatCmpEQCodegen(CodeGenerationVisitor &cv,
                                                 OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateFCmpOEQ(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::FloatCmpNEQCodegen(CodeGenerationVisitor &cv,
                                                  OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateFCmpONE(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::FloatCmpGTCodegen(CodeGenerationVisitor &cv,
                                                 OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateFCmpOGT(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::FloatCmpLTCodegen(CodeGenerationVisitor &cv,
                                                 OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateFCmpOLT(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::FloatCmpGEQCodegen(CodeGenerationVisitor &cv,
                                                  OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateFCmpOGE(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
}

llvm::Value *operator_codegen::FloatCmpLEQCodegen(CodeGenerationVisitor &cv,
                                                  OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);

  auto i1_result = builder->CreateFCmpOLE(lhs, rhs);
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*llvm_context));
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

  // Runtime error if denominator is 0.
  llvm::Function *parent_function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *zero_denom_block = llvm::BasicBlock::Create(*llvm_context);
  llvm::BasicBlock *nonzero_denom_block =
      llvm::BasicBlock::Create(*llvm_context);

  llvm::Value *int64_zero =
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 0);
  llvm::Value *zero_denom_cmp = builder->CreateICmpEQ(denom, int64_zero);
  builder->CreateCondBr(zero_denom_cmp, zero_denom_block, nonzero_denom_block);

  // Zero denominator case.
  parent_function->insert(parent_function->end(), zero_denom_block);
  builder->SetInsertPoint(zero_denom_block);
  cv.GenerateRuntimeErrorWithMessage("zero denominator");

  // We continue as usual for non-zero denominators.
  parent_function->insert(parent_function->end(), nonzero_denom_block);
  builder->SetInsertPoint(nonzero_denom_block);

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

llvm::Value *operator_codegen::RationalNopCodegen(CodeGenerationVisitor &cv,
                                                  OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);
  llvm::Type *int128_type = llvm::Type::getInt128Ty(*llvm_context);

  // We need to extract the operand numerator and denominator as 64-bit
  // integers.
  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *num = builder->CreateShl(operand, 64);
  num = builder->CreateLShr(num, 64);
  num = builder->CreateTrunc(num, int64_type);
  llvm::Value *denom = builder->CreateLShr(operand, 64);
  denom = builder->CreateTrunc(denom, int64_type);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (!next_op || !next_op->IsArithmeticOperation()) {
    // If the parent is not another arithmetic operation, simplify the rational
    // number now.
    SimplifyRational(num, denom);
  }

  num = builder->CreateZExt(num, int128_type);
  denom = builder->CreateZExt(denom, int128_type);

  // Shift denominator left by 64 bits in preparation for storage.
  denom = builder->CreateShl(denom, 64);

  llvm::Value *result = builder->CreateOr(num, denom);

  return result;
}

llvm::Value *operator_codegen::RationalNegationCodegen(
    CodeGenerationVisitor &cv, OperatorNode &node) {
  // Rational numbers are stored in a 128-bit integer. First 64-bits are
  // represent the numerator, last 64-bits represent the denominator.
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);
  llvm::Type *int128_type = llvm::Type::getInt128Ty(*llvm_context);

  // We need to extract the operand numerator and denominator as 64-bit
  // integers.
  llvm::Value *operand = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *num = builder->CreateShl(operand, 64);
  num = builder->CreateLShr(num, 64);
  num = builder->CreateTrunc(num, int64_type);
  llvm::Value *denom = builder->CreateLShr(operand, 64);
  denom = builder->CreateTrunc(denom, int64_type);

  // Negate the numerator.
  num = builder->CreateSub(
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*llvm_context), 0), num);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (!next_op || !next_op->IsArithmeticOperation()) {
    // If the parent is not another arithmetic operation, simplify the rational
    // number now.
    SimplifyRational(num, denom);
  }

  num = builder->CreateZExt(num, int128_type);
  denom = builder->CreateZExt(denom, int128_type);

  // Shift denominator left by 64 bits in preparation for storage.
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

  // Multiply LHS numerator by RHS denominator.
  lhs_num = builder->CreateMul(lhs_num, rhs_denom);

  // Multiply RHS numerator by LHS denominator.
  rhs_num = builder->CreateMul(rhs_num, lhs_denom);

  // a/b + c/d = (ad + cb) / (bd)
  llvm::Value *result_num = builder->CreateAdd(lhs_num, rhs_num);
  llvm::Value *result_denom = builder->CreateMul(lhs_denom, rhs_denom);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (!next_op || !next_op->IsArithmeticOperation()) {
    // If the parent is not another arithmetic operation, simplify the rational
    // number now.
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

  // Multiply LHS numerator by RHS denominator.
  lhs_num = builder->CreateMul(lhs_num, rhs_denom);

  // Multiply RHS numerator by LHS denominator.
  rhs_num = builder->CreateMul(rhs_num, lhs_denom);

  // a/b - c/d = (ad - cb) / (bd)
  llvm::Value *result_num = builder->CreateSub(lhs_num, rhs_num);
  llvm::Value *result_denom = builder->CreateMul(lhs_denom, rhs_denom);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (!next_op || !next_op->IsArithmeticOperation()) {
    // If the parent is not another arithmetic operation, simplify the rational
    // number now.
    SimplifyRational(result_num, result_denom);
  }

  result_num = builder->CreateZExt(result_num, int128_type);
  result_denom = builder->CreateZExt(result_denom, int128_type);

  // Shift denominator left by 64 bits in preparation for storage.
  result_denom = builder->CreateShl(result_denom, 64);

  llvm::Value *result = builder->CreateOr(result_num, result_denom);
  return result;
}

llvm::Value *operator_codegen::RationalMultiplyCodegen(
    CodeGenerationVisitor &cv, OperatorNode &node) {
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

  // (a/b) * (c/d) = (ac)/(bd)
  llvm::Value *result_num = builder->CreateMul(lhs_num, rhs_num);
  llvm::Value *result_denom = builder->CreateMul(lhs_denom, rhs_denom);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (!next_op || !next_op->IsArithmeticOperation()) {
    // If the parent is not another arithmetic operation, simplify the rational
    // number now.
    SimplifyRational(result_num, result_denom);
  }

  result_num = builder->CreateZExt(result_num, int128_type);
  result_denom = builder->CreateZExt(result_denom, int128_type);

  // Shift denominator left by 64 bits in preparation for storage.
  result_denom = builder->CreateShl(result_denom, 64);

  llvm::Value *result = builder->CreateOr(result_num, result_denom);

  return result;
}

llvm::Value *operator_codegen::RationalDivideCodegen(CodeGenerationVisitor &cv,
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

  // (a/b) / (c/d) = (ad) / (bc)
  llvm::Value *result_num = builder->CreateMul(lhs_num, rhs_denom);
  llvm::Value *result_denom = builder->CreateMul(lhs_denom, rhs_num);

  auto next_op = dynamic_cast<OperatorNode *>(node.GetParent());
  if (!next_op || !next_op->IsArithmeticOperation()) {
    // If the parent is not another arithmetic operation, simplify the rational
    // number now.
    SimplifyRational(result_num, result_denom);
  }

  result_num = builder->CreateZExt(result_num, int128_type);
  result_denom = builder->CreateZExt(result_denom, int128_type);

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
  return builder->CreateICmpEQ(lhs, rhs);
}

llvm::Value *operator_codegen::RationalCmpNEQCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(cv);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(cv);
  return builder->CreateICmpNE(lhs, rhs);
}

llvm::Value *operator_codegen::RationalCmpGTCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);

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

  // Given rational numbers a/b and c/d, compare ad > cb.
  llvm::Value *cmp_op1 = builder->CreateMul(lhs_num, rhs_denom);
  llvm::Value *cmp_op2 = builder->CreateMul(rhs_num, lhs_denom);
  return builder->CreateICmpSGT(cmp_op1, cmp_op2);
}

llvm::Value *operator_codegen::RationalCmpLTCodegen(CodeGenerationVisitor &cv,
                                                    OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);

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

  // Given rational numbers a/b and c/d, compare ad < cb.
  llvm::Value *cmp_op1 = builder->CreateMul(lhs_num, rhs_denom);
  llvm::Value *cmp_op2 = builder->CreateMul(rhs_num, lhs_denom);
  return builder->CreateICmpSLT(cmp_op1, cmp_op2);
}

llvm::Value *operator_codegen::RationalCmpGEQCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);

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

  // Given rational numbers a/b and c/d, compare ad >= cb.
  llvm::Value *cmp_op1 = builder->CreateMul(lhs_num, rhs_denom);
  llvm::Value *cmp_op2 = builder->CreateMul(rhs_num, lhs_denom);
  return builder->CreateICmpSGE(cmp_op1, cmp_op2);
}

llvm::Value *operator_codegen::RationalCmpLEQCodegen(CodeGenerationVisitor &cv,
                                                     OperatorNode &node) {
  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);

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

  // Given rational numbers a/b and c/d, compare ad =< cb.
  llvm::Value *cmp_op1 = builder->CreateMul(lhs_num, rhs_denom);
  llvm::Value *cmp_op2 = builder->CreateMul(rhs_num, lhs_denom);
  return builder->CreateICmpSLE(cmp_op1, cmp_op2);
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
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  // Size of array is the first field of the PunktArray struct.
  llvm::Value *PunktArray_ptr = node.GetChild(0)->GenerateCode(cv);
  return builder->CreateLoad(llvm::Type::getInt64Ty(*llvm_context),
                             PunktArray_ptr, "PunktArray_size");
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
      builder->CreateICmpSLT(denominator, int64_zero);

  // Create a value holding the numerator with flipped sign.
  llvm::Value *numerator_flipped_sign =
      builder->CreateSub(int64_zero, numerator);

  // Select instruction to flip the sign of the denominator.
  numerator = builder->CreateSelect(denom_negative_check,
                                    numerator_flipped_sign, numerator);

  // Set denominator to abs(denominator).
  llvm::Function *abs_intrinsic = llvm::Intrinsic::getDeclaration(
      module, llvm::Intrinsic::abs, {int64_type});
  denominator = builder->CreateCall(abs_intrinsic, {denominator, i1_zero});

  // Get GCD(numerator, denominator). Use abs(numerator) to compute the GCD.
  llvm::Value *abs_numerator =
      builder->CreateCall(abs_intrinsic, {numerator, i1_zero});
  llvm::Value *gcd = GenerateGCD(abs_numerator, denominator);

  // Divide numerator and denominator by GCD.
  numerator = builder->CreateSDiv(numerator, gcd);
  denominator = builder->CreateSDiv(denominator, gcd);
}

llvm::Value *GenerateGCD(llvm::Value *a, llvm::Value *b) {
  // The LLVM IR generated by this function implements the GCD algorithm below.
  // Source: https://en.wikipedia.org/wiki/Euclidean_algorithm#Implementations
  //
  // gcd(a, b):
  //   while (b != 0):
  //     t = b
  //     b = a % b
  //     a = t
  //   return a
  //
  //
  // Pseudo-IR for the GCD algorithm:
  //
  // gcd_before:
  //   ...
  //   gcdarg1 = add 0, arg1
  //   gcdarg2 = add 0, arg2
  //   br label %gcd_loop_cond
  //
  // gcd_loop_cond:
  //   %gcda = phi [%gcdarg1, %gcd_before], [%nexta, %gcd_loop_body]
  //   %gcdb = phi [%gcdarg2, %gcd_before], [%nextb, %gcd_loop_body]
  //   %condcheck = icmp ne %gcdb, 0
  //   br %condcheck, label %gcd_loop_body, label %gcd_loop_end
  //
  // gcd_loop_body:
  //   %gcdt = add 0, %gcdb
  //   %nextb = urem %gcda, %gcdb
  //   %nexta = add 0, %gcdt
  //   br label %gcd_loop_cond
  //
  // gcd_loop_end:
  //   ; Here, %gcda is the GCD.

  CodegenContext *codegen_context = CodegenContext::Get();
  llvm::LLVMContext *llvm_context = codegen_context->GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context->GetIRBuilder();

  llvm::Type *int64_type = llvm::Type::getInt64Ty(*llvm_context);
  llvm::Value *int64_zero = llvm::ConstantInt::get(int64_type, 0);

  // Create the necessary basic blocks.
  llvm::Function *parent_function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *gcd_before = builder->GetInsertBlock();
  llvm::BasicBlock *gcd_loop_cond = llvm::BasicBlock::Create(*llvm_context);
  llvm::BasicBlock *gcd_loop_body = llvm::BasicBlock::Create(*llvm_context);
  llvm::BasicBlock *gcd_loop_end = llvm::BasicBlock::Create(*llvm_context);

  // gcd_before:
  llvm::Value *gcdarg1 = builder->CreateAdd(a, int64_zero);
  llvm::Value *gcdarg2 = builder->CreateAdd(b, int64_zero);
  builder->CreateBr(gcd_loop_cond);

  // gcd_loop_cond:
  parent_function->insert(parent_function->end(), gcd_loop_cond);
  builder->SetInsertPoint(gcd_loop_cond);
  llvm::PHINode *gcda = builder->CreatePHI(int64_type, 2);
  llvm::PHINode *gcdb = builder->CreatePHI(int64_type, 2);
  gcda->addIncoming(gcdarg1, gcd_before);
  gcdb->addIncoming(gcdarg2, gcd_before);
  llvm::Value *condcheck = builder->CreateICmpNE(gcdb, int64_zero);
  builder->CreateCondBr(condcheck, gcd_loop_body, gcd_loop_end);

  // gcd_loop_body:
  parent_function->insert(parent_function->end(), gcd_loop_body);
  builder->SetInsertPoint(gcd_loop_body);
  llvm::Value *gcdt = builder->CreateAdd(gcdb, int64_zero);
  llvm::Value *nextb = builder->CreateURem(gcda, gcdb);
  llvm::Value *nexta = builder->CreateAdd(gcdt, int64_zero);
  gcda->addIncoming(nexta, gcd_loop_body);
  gcdb->addIncoming(nextb, gcd_loop_body);
  builder->CreateBr(gcd_loop_cond);

  // gcd_loop_end:
  parent_function->insert(parent_function->end(), gcd_loop_end);
  builder->SetInsertPoint(gcd_loop_end);
  return gcda;
}
