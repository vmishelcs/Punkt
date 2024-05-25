#ifndef OPERATOR_CODEGEN_H_
#define OPERATOR_CODEGEN_H_

#include <parse_node/parse_nodes/operator_node.h>

#include "code_generation_visitor.h"

namespace operator_codegen {

// ---- Assignment -------------------------------------------------------------
llvm::Value *AssignmentCodegen(CodeGenerationVisitor &codegen_visitor,
                               OperatorNode &operator_node);

//===- NOP ----------------------------------------------------------------===//
llvm::Value *UnaryNop(CodeGenerationVisitor &codegen_visitor,
                      OperatorNode &operator_node);

//===- Booleans -----------------------------------------------------------===//
llvm::Value *BooleanNegationCodegen(CodeGenerationVisitor &codegen_visitor,
                                    OperatorNode &operator_node);
llvm::Value *BooleanCmpEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                 OperatorNode &operator_node);
llvm::Value *BooleanCmpNEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                  OperatorNode &operator_node);
llvm::Value *BooleanAndCodegen(CodeGenerationVisitor &codegen_visitor,
                               OperatorNode &operator_node);
llvm::Value *BooleanOrCodegen(CodeGenerationVisitor &codegen_visitor,
                              OperatorNode &operator_node);

//===- Characters ---------------------------------------------------------===//
llvm::Value *CharacterCmpEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                   OperatorNode &operator_node);
llvm::Value *CharacterCmpNEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                    OperatorNode &operator_node);
llvm::Value *CharacterCmpGTCodegen(CodeGenerationVisitor &codegen_visitor,
                                   OperatorNode &operator_node);
llvm::Value *CharacterCmpLTCodegen(CodeGenerationVisitor &codegen_visitor,
                                   OperatorNode &operator_node);
llvm::Value *CharacterCmpGEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                    OperatorNode &operator_node);
llvm::Value *CharacterCmpLEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                    OperatorNode &operator_node);

//===- Integers -----------------------------------------------------------===//
llvm::Value *IntegerNegationCodegen(CodeGenerationVisitor &codegen_visitor,
                                    OperatorNode &operator_node);
llvm::Value *IntegerAddCodegen(CodeGenerationVisitor &codegen_visitor,
                               OperatorNode &operator_node);
llvm::Value *IntegerSubtractCodegen(CodeGenerationVisitor &codegen_visitor,
                                    OperatorNode &operator_node);
llvm::Value *IntegerMultiplyCodegen(CodeGenerationVisitor &codegen_visitor,
                                    OperatorNode &operator_node);
llvm::Value *IntegerDivideCodegen(CodeGenerationVisitor &codegen_visitor,
                                  OperatorNode &operator_node);
llvm::Value *IntegerModuloCodegen(CodeGenerationVisitor &codegen_visitor,
                                  OperatorNode &operator_node);
llvm::Value *IntegerCmpEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                 OperatorNode &operator_node);
llvm::Value *IntegerCmpNEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                  OperatorNode &operator_node);
llvm::Value *IntegerCmpGTCodegen(CodeGenerationVisitor &codegen_visitor,
                                 OperatorNode &operator_node);
llvm::Value *IntegerCmpLTCodegen(CodeGenerationVisitor &codegen_visitor,
                                 OperatorNode &operator_node);
llvm::Value *IntegerCmpGEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                  OperatorNode &operator_node);
llvm::Value *IntegerCmpLEQCodegen(CodeGenerationVisitor &codegen_visitor,
                                  OperatorNode &operator_node);

//===- Rationals ----------------------------------------------------------===//
llvm::Value *OverOperatorCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalAddCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalSubtractCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalMultiplyCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalDivideCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalCmpEQCodegen(CodeGenerationVisitor &, OperatorNode &);
llvm::Value *RationalCmpNEQCodegen(CodeGenerationVisitor &, OperatorNode &);

//===- Arrays -------------------------------------------------------------===//
llvm::Value *ArrayIndexingCodegen(CodeGenerationVisitor &codegen_visitor,
                                  OperatorNode &operator_node);
llvm::Value *ArraySizeofCodegen(CodeGenerationVisitor &codegen_visitor,
                                OperatorNode &operator_node);

};  // namespace operator_codegen

#endif  // OPERATOR_CODEGEN_H_
