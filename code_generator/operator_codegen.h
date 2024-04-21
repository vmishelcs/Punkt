#ifndef OPERATOR_CODEGEN_H_
#define OPERATOR_CODEGEN_H_

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

namespace operator_codegen {

llvm::Value *UnaryNop(llvm::LLVMContext *context, llvm::IRBuilder<> *builder,
                      llvm::Value *operand);

// ---- Booleans -------------------------------------------------------------
llvm::Value *BooleanCmpEQCodeGenerator(llvm::LLVMContext *context,
                                       llvm::IRBuilder<> *builder,
                                       llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *BooleanCmpNEQCodeGenerator(llvm::LLVMContext *context,
                                        llvm::IRBuilder<> *builder,
                                        llvm::Value *lhs, llvm::Value *rhs);

// ---- Characters -----------------------------------------------------------
llvm::Value *CharacterCmpEQCodeGenerator(llvm::LLVMContext *context,
                                         llvm::IRBuilder<> *builder,
                                         llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CharacterCmpNEQCodeGenerator(llvm::LLVMContext *context,
                                          llvm::IRBuilder<> *builder,
                                          llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CharacterCmpGTCodeGenerator(llvm::LLVMContext *context,
                                         llvm::IRBuilder<> *builder,
                                         llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CharacterCmpLTCodeGenerator(llvm::LLVMContext *context,
                                         llvm::IRBuilder<> *builder,
                                         llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CharacterCmpGEQCodeGenerator(llvm::LLVMContext *context,
                                          llvm::IRBuilder<> *builder,
                                          llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CharacterCmpLEQCodeGenerator(llvm::LLVMContext *context,
                                          llvm::IRBuilder<> *builder,
                                          llvm::Value *lhs, llvm::Value *rhs);

// ---- Integers -------------------------------------------------------------
llvm::Value *IntegerNegationCodeGenerator(llvm::LLVMContext *context,
                                          llvm::IRBuilder<> *builder,
                                          llvm::Value *operand);
llvm::Value *IntegerAddCodeGenerator(llvm::LLVMContext *context,
                                     llvm::IRBuilder<> *builder,
                                     llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerSubtractCodeGenerator(llvm::LLVMContext *context,
                                          llvm::IRBuilder<> *builder,
                                          llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerMultiplyCodeGenerator(llvm::LLVMContext *context,
                                          llvm::IRBuilder<> *builder,
                                          llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerDivideCodeGenerator(llvm::LLVMContext *context,
                                        llvm::IRBuilder<> *builder,
                                        llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerCmpEQCodeGenerator(llvm::LLVMContext *context,
                                       llvm::IRBuilder<> *builder,
                                       llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerCmpNEQCodeGenerator(llvm::LLVMContext *context,
                                        llvm::IRBuilder<> *builder,
                                        llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerCmpGTCodeGenerator(llvm::LLVMContext *context,
                                       llvm::IRBuilder<> *builder,
                                       llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerCmpLTCodeGenerator(llvm::LLVMContext *context,
                                       llvm::IRBuilder<> *builder,
                                       llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerCmpGEQCodeGenerator(llvm::LLVMContext *context,
                                        llvm::IRBuilder<> *builder,
                                        llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerCmpLEQCodeGenerator(llvm::LLVMContext *context,
                                        llvm::IRBuilder<> *builder,
                                        llvm::Value *lhs, llvm::Value *rhs);

};  // namespace operator_codegen

#endif  // OPERATOR_CODEGEN_H_
