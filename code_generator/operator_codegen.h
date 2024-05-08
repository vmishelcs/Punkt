#ifndef OPERATOR_CODEGEN_H_
#define OPERATOR_CODEGEN_H_

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

namespace operator_codegen {

// ---- Assignment -------------------------------------------------------------
llvm::Value *AssignmentCodegen(llvm::LLVMContext *context,
                               llvm::IRBuilder<> *builder, llvm::Value *target,
                               llvm::Value *new_value);

// ---- NOP --------------------------------------------------------------------
llvm::Value *UnaryNop(llvm::LLVMContext *context, llvm::IRBuilder<> *builder,
                      llvm::Value *operand);

// ---- Booleans ---------------------------------------------------------------
llvm::Value *BooleanCmpEQCodegen(llvm::LLVMContext *context,
                                 llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                 llvm::Value *rhs);
llvm::Value *BooleanCmpNEQCodegen(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                  llvm::Value *rhs);
llvm::Value *BooleanBoolANDCodegen(llvm::LLVMContext *context,
                                   llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                   llvm::Value *rhs);
llvm::Value *BooleanBoolORCodegen(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                  llvm::Value *rhs);

// ---- Characters -------------------------------------------------------------
llvm::Value *CharacterCmpEQCodegen(llvm::LLVMContext *context,
                                   llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                   llvm::Value *rhs);
llvm::Value *CharacterCmpNEQCodegen(llvm::LLVMContext *context,
                                    llvm::IRBuilder<> *builder,
                                    llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CharacterCmpGTCodegen(llvm::LLVMContext *context,
                                   llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                   llvm::Value *rhs);
llvm::Value *CharacterCmpLTCodegen(llvm::LLVMContext *context,
                                   llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                   llvm::Value *rhs);
llvm::Value *CharacterCmpGEQCodegen(llvm::LLVMContext *context,
                                    llvm::IRBuilder<> *builder,
                                    llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *CharacterCmpLEQCodegen(llvm::LLVMContext *context,
                                    llvm::IRBuilder<> *builder,
                                    llvm::Value *lhs, llvm::Value *rhs);

// ---- Integers ---------------------------------------------------------------
llvm::Value *IntegerNegationCodegen(llvm::LLVMContext *context,
                                    llvm::IRBuilder<> *builder,
                                    llvm::Value *operand);
llvm::Value *IntegerAddCodegen(llvm::LLVMContext *context,
                               llvm::IRBuilder<> *builder, llvm::Value *lhs,
                               llvm::Value *rhs);
llvm::Value *IntegerSubtractCodegen(llvm::LLVMContext *context,
                                    llvm::IRBuilder<> *builder,
                                    llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerMultiplyCodegen(llvm::LLVMContext *context,
                                    llvm::IRBuilder<> *builder,
                                    llvm::Value *lhs, llvm::Value *rhs);
llvm::Value *IntegerDivideCodegen(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                  llvm::Value *rhs);
llvm::Value *IntegerModulusCodegen(llvm::LLVMContext *context,
                                   llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                   llvm::Value *rhs);
llvm::Value *IntegerCmpEQCodegen(llvm::LLVMContext *context,
                                 llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                 llvm::Value *rhs);
llvm::Value *IntegerCmpNEQCodegen(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                  llvm::Value *rhs);
llvm::Value *IntegerCmpGTCodegen(llvm::LLVMContext *context,
                                 llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                 llvm::Value *rhs);
llvm::Value *IntegerCmpLTCodegen(llvm::LLVMContext *context,
                                 llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                 llvm::Value *rhs);
llvm::Value *IntegerCmpGEQCodegen(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                  llvm::Value *rhs);
llvm::Value *IntegerCmpLEQCodegen(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *builder, llvm::Value *lhs,
                                  llvm::Value *rhs);

};  // namespace operator_codegen

#endif  // OPERATOR_CODEGEN_H_
