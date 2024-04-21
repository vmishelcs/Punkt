#ifndef OPERATOR_CODE_GENERATOR_H_
#define OPERATOR_CODE_GENERATOR_H_

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

class OperatorCodeGenerator {
 public:
  static llvm::Value *UnaryNoOp(llvm::LLVMContext *context,
                                llvm::IRBuilder<> *builder,
                                llvm::Value *operand);

  // ---- Booleans
  // -------------------------------------------------------------------------
  static llvm::Value *BooleanCmpEqCodeGenerator(llvm::LLVMContext *context,
                                                llvm::IRBuilder<> *builder,
                                                llvm::Value *lhs,
                                                llvm::Value *rhs);
  static llvm::Value *BooleanCmpNeqCodeGenerator(llvm::LLVMContext *context,
                                                 llvm::IRBuilder<> *builder,
                                                 llvm::Value *lhs,
                                                 llvm::Value *rhs);

  // ---- Characters
  // -----------------------------------------------------------------------
  static llvm::Value *CharacterCmpEQCodeGenerator(llvm::LLVMContext *context,
                                                  llvm::IRBuilder<> *builder,
                                                  llvm::Value *lhs,
                                                  llvm::Value *rhs);
  static llvm::Value *CharacterCmpNEQCodeGenerator(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
                                                   llvm::Value *rhs);
  static llvm::Value *CharacterCmpGTCodeGenerator(llvm::LLVMContext *context,
                                                  llvm::IRBuilder<> *builder,
                                                  llvm::Value *lhs,
                                                  llvm::Value *rhs);
  static llvm::Value *CharacterCmpLTCodeGenerator(llvm::LLVMContext *context,
                                                  llvm::IRBuilder<> *builder,
                                                  llvm::Value *lhs,
                                                  llvm::Value *rhs);
  static llvm::Value *CharacterCmpGEQCodeGenerator(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
                                                   llvm::Value *rhs);
  static llvm::Value *CharacterCmpLEQCodeGenerator(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
                                                   llvm::Value *rhs);

  // ---- Integers
  // -------------------------------------------------------------------------
  static llvm::Value *IntegerNegationCodeGenerator(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *operand);
  static llvm::Value *IntegerAddCodeGenerator(llvm::LLVMContext *context,
                                              llvm::IRBuilder<> *builder,
                                              llvm::Value *lhs,
                                              llvm::Value *rhs);
  static llvm::Value *IntegerSubtractCodeGenerator(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
                                                   llvm::Value *rhs);
  static llvm::Value *IntegerMultiplyCodeGenerator(llvm::LLVMContext *context,
                                                   llvm::IRBuilder<> *builder,
                                                   llvm::Value *lhs,
                                                   llvm::Value *rhs);
  static llvm::Value *IntegerDivideCodeGenerator(llvm::LLVMContext *context,
                                                 llvm::IRBuilder<> *builder,
                                                 llvm::Value *lhs,
                                                 llvm::Value *rhs);
  static llvm::Value *IntegerCmpEQCodeGenerator(llvm::LLVMContext *context,
                                                llvm::IRBuilder<> *builder,
                                                llvm::Value *lhs,
                                                llvm::Value *rhs);
  static llvm::Value *IntegerCmpNEQCodeGenerator(llvm::LLVMContext *context,
                                                 llvm::IRBuilder<> *builder,
                                                 llvm::Value *lhs,
                                                 llvm::Value *rhs);
  static llvm::Value *IntegerCmpGTCodeGenerator(llvm::LLVMContext *context,
                                                llvm::IRBuilder<> *builder,
                                                llvm::Value *lhs,
                                                llvm::Value *rhs);
  static llvm::Value *IntegerCmpLTCodeGenerator(llvm::LLVMContext *context,
                                                llvm::IRBuilder<> *builder,
                                                llvm::Value *lhs,
                                                llvm::Value *rhs);
  static llvm::Value *IntegerCmpGEQCodeGenerator(llvm::LLVMContext *context,
                                                 llvm::IRBuilder<> *builder,
                                                 llvm::Value *lhs,
                                                 llvm::Value *rhs);
  static llvm::Value *IntegerCmpLEQCodeGenerator(llvm::LLVMContext *context,
                                                 llvm::IRBuilder<> *builder,
                                                 llvm::Value *lhs,
                                                 llvm::Value *rhs);
};

#endif  // OPERATOR_CODE_GENERATOR_H_
