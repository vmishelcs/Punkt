#include "operator_codegen.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

#include "codegen_context.h"

/******************************************************************************
 *                                 Assignment                                 *
 ******************************************************************************/
llvm::Value *operator_codegen::AssignmentCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  llvm::Value *target = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *new_value = node.GetChild(1)->GenerateCode(codegen_visitor);

  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  builder->CreateStore(new_value, target);
  return new_value;
}

/******************************************************************************
 *                                    NOP                                     *
 ******************************************************************************/
llvm::Value *operator_codegen::UnaryNop(CodeGenerationVisitor &codegen_visitor,
                                        OperatorNode &node) {
  llvm::Value *operand = node.GetChild(0)->GenerateCode(codegen_visitor);
  return operand;
}

/******************************************************************************
 *                                  Booleans                                  *
 ******************************************************************************/
llvm::Value *operator_codegen::BooleanNegationCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *operand = node.GetChild(0)->GenerateCode(codegen_visitor);

  auto operand_trunc = builder->CreateTrunc(
      operand, llvm::Type::getInt1Ty(*context), "trunctmp");

  // XOR the operand with 1.
  auto i1_result = builder->CreateXor(
      operand_trunc,
      llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), true), "xortmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::BooleanCmpEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

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

llvm::Value *operator_codegen::BooleanCmpNEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

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

llvm::Value *operator_codegen::BooleanAndCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto lhs_trunc =
      builder->CreateTrunc(lhs, llvm::Type::getInt1Ty(*context), "trunctmp");
  auto rhs_trunc =
      builder->CreateTrunc(rhs, llvm::Type::getInt1Ty(*context), "trunctmp");

  auto i1_result = builder->CreateAnd(lhs_trunc, rhs_trunc, "andtmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::BooleanOrCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

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
llvm::Value *operator_codegen::CharacterCmpEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpEQ(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpNEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpNE(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpGTCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSGT(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpLTCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSLT(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpGEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto lhs_sext =
      builder->CreateSExt(lhs, llvm::Type::getInt32Ty(*context), "sexttmp");
  auto rhs_sext =
      builder->CreateSExt(rhs, llvm::Type::getInt32Ty(*context), "sexttmp");

  auto i1_result = builder->CreateICmpSGE(lhs_sext, rhs_sext, "cmptmp");

  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::CharacterCmpLEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

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
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *operand = node.GetChild(0)->GenerateCode(codegen_visitor);

  auto neg_val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), -1);
  return builder->CreateMul(operand, neg_val, "negtmp");
}

llvm::Value *operator_codegen::IntegerAddCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  return builder->CreateAdd(lhs, rhs, "addtmp");
}

llvm::Value *operator_codegen::IntegerSubtractCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  return builder->CreateSub(lhs, rhs, "subtmp");
}

llvm::Value *operator_codegen::IntegerMultiplyCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  return builder->CreateMul(lhs, rhs, "multmp");
}

llvm::Value *operator_codegen::IntegerDivideCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  return builder->CreateSDiv(lhs, rhs, "divtmp");
}

llvm::Value *operator_codegen::IntegerModuloCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  return builder->CreateSRem(lhs, rhs, "modtmp");
}

llvm::Value *operator_codegen::IntegerCmpEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto i1_result = builder->CreateICmpEQ(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpNEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto i1_result = builder->CreateICmpNE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpGTCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto i1_result = builder->CreateICmpSGT(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpLTCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto i1_result = builder->CreateICmpSLT(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpGEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto i1_result = builder->CreateICmpSGE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

llvm::Value *operator_codegen::IntegerCmpLEQCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  CodegenContext &codegen_context = CodegenContext::Get();
  llvm::LLVMContext *context = codegen_context.GetLLVMContext();
  llvm::IRBuilder<> *builder = codegen_context.GetIRBuilder();

  llvm::Value *lhs = node.GetChild(0)->GenerateCode(codegen_visitor);
  llvm::Value *rhs = node.GetChild(1)->GenerateCode(codegen_visitor);

  auto i1_result = builder->CreateICmpSLE(lhs, rhs, "cmptmp");
  return builder->CreateZExt(i1_result, llvm::Type::getInt8Ty(*context),
                             "zexttmp");
}

/******************************************************************************
 *                                   Arrays                                   *
 ******************************************************************************/
llvm::Value *operator_codegen::ArrayAllocCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  return nullptr;
}

llvm::Value *operator_codegen::ArrayIndexingCodegen(
    CodeGenerationVisitor &codegen_visitor, OperatorNode &node) {
  return nullptr;
}
