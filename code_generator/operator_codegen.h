#ifndef OPERATOR_CODEGEN_H_
#define OPERATOR_CODEGEN_H_

#include <llvm/IR/Value.h>

class CodeGenerationVisitor;
class OperatorNode;

namespace operator_codegen {

//===- Assignment ---------------------------------------------------------===//
llvm::Value *AssignmentCodegen(CodeGenerationVisitor &, OperatorNode &);

//===- NOP ----------------------------------------------------------------===//
llvm::Value *UnaryNop(CodeGenerationVisitor &, OperatorNode &);

//===- Booleans -----------------------------------------------------------===//
llvm::Value *BooleanNegationCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *BooleanCmpEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *BooleanCmpNEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *BooleanAndCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *BooleanOrCodegen(CodeGenerationVisitor &, OperatorNode &);

//===- Characters ---------------------------------------------------------===//
llvm::Value *CharacterCmpEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *CharacterCmpNEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *CharacterCmpGTCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *CharacterCmpLTCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *CharacterCmpGEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *CharacterCmpLEQCodegen(CodeGenerationVisitor &, OperatorNode &);

//===- Integers -----------------------------------------------------------===//
llvm::Value *IntegerNegationCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerAddCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerSubtractCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerMultiplyCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerDivideCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerModuloCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerCmpEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerCmpNEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerCmpGTCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerCmpLTCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerCmpGEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *IntegerCmpLEQCodegen(CodeGenerationVisitor &, OperatorNode &);

//===- Floating point -----------------------------------------------------===//
llvm::Value *FloatNegationCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatAddCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatSubtractCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatMultiplyCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatDivideCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatCmpEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatCmpNEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatCmpGTCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatCmpLTCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatCmpGEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *FloatCmpLEQCodegen(CodeGenerationVisitor &, OperatorNode &);

//===- Rationals ----------------------------------------------------------===//
llvm::Value *OverOperatorCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalNopCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalNegationCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalAddCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalSubtractCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalMultiplyCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalDivideCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalCmpEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalCmpNEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalCmpGTCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalCmpLTCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalCmpGEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalCmpLEQCodegen(CodeGenerationVisitor &, OperatorNode &);

//===- Arrays -------------------------------------------------------------===//
llvm::Value *ArrayIndexingCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *ArraySizeofCodegen(CodeGenerationVisitor &, OperatorNode &);

};  // namespace operator_codegen

#endif  // OPERATOR_CODEGEN_H_
