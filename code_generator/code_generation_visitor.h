#ifndef CODE_GENERATION_VISITOR_H_
#define CODE_GENERATION_VISITOR_H_

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <semantic_analyzer/types/array_type.h>
#include <semantic_analyzer/types/base_type.h>
#include <semantic_analyzer/types/lambda_type.h>

#include <map>
#include <memory>
#include <string>

#include "codegen_context.h"

class CodeGenerationVisitor : public ParseNodeIRVisitor {
 public:
  CodeGenerationVisitor(std::string module_id);

  void WriteIRToFD(int fd);

  //===- Statements -------------------------------------------------------===//
  virtual llvm::Value *GenerateCode(DeclarationStatementNode &) override;
  virtual llvm::Value *GenerateCode(ExpressionStatementNode &) override;
  virtual llvm::Value *GenerateCode(IfStatementNode &) override;
  virtual llvm::Value *GenerateCode(WhileStatementNode &) override;
  virtual llvm::Value *GenerateCode(ForStatementNode &) override;
  virtual llvm::Value *GenerateCode(DeallocStatementNode &) override;
  virtual llvm::Value *GenerateCode(ReturnStatementNode &) override;
  virtual llvm::Value *GenerateCode(PrintStatementNode &) override;

  virtual llvm::Value *GenerateCode(CodeBlockNode &) override;
  virtual llvm::Value *GenerateCode(FunctionDefinitionNode &) override;
  virtual llvm::Value *GenerateCode(LambdaInvocationNode &) override;
  virtual llvm::Value *GenerateCode(LambdaNode &) override;
  virtual llvm::Value *GenerateCode(LambdaParameterNode &) override;
  virtual llvm::Value *GenerateCode(MainNode &) override;
  virtual llvm::Value *GenerateCode(OperatorNode &) override;
  virtual llvm::Value *GenerateCode(ProgramNode &) override;

  virtual llvm::Value *GenerateCode(ErrorNode &) override;
  virtual llvm::Value *GenerateCode(NopNode &) override;
  virtual llvm::Value *GenerateCode(IdentifierNode &) override;

  //===- Literals ---------------------------------------------------------===//
  virtual llvm::Value *GenerateCode(BooleanLiteralNode &) override;
  virtual llvm::Value *GenerateCode(CharacterLiteralNode &) override;
  virtual llvm::Value *GenerateCode(IntegerLiteralNode &) override;
  virtual llvm::Value *GenerateCode(StringLiteralNode &) override;

  //===- Arrays -----------------------------------------------------------===//
  virtual llvm::Value *GenerateCode(AllocExpressionNode &) override;
  virtual llvm::Value *GenerateCode(PopulatedArrayExpressionNode &) override;

  //===- Type nodes -------------------------------------------------------===//
  virtual llvm::Value *GenerateCode(BaseTypeNode &) override;
  virtual llvm::Value *GenerateCode(ArrayTypeNode &) override;
  virtual llvm::Value *GenerateCode(LambdaTypeNode &) override;

  const std::string &GetPunktArrayStructName() const;

  void GenerateRuntimeErrorWithMessage(const std::string &message);

 private:
  /// @brief Create an `AllocaInst` in the entry block of a specified function
  /// for a provided variable.
  /// @param function Function the variable resides in.
  /// @param identifier Identifier of the variable.
  /// @param llvm_type `llvm::Type` of the variable.
  /// @return `llvm::AllocaInst` that allocates memory for the specified
  /// variable within the provided function.
  llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *function,
                                           const std::string &identifier,
                                           llvm::Type *llvm_type);

  /// @brief Get an `llvm::Value` pointer to the provided string.
  /// @param str String to be created in the IR.
  /// @return `llvm::Value` pointer to the specified string.
  llvm::Value *GetOrCreateString(const std::string &str);

  /// @brief Check if the previous instruction was a block terminator.
  /// @return `true` if previous instruction was a block terminator, `false
  /// otherwise.
  bool IsPreviousInstructionBlockTerminator();

  /// Helpful C standard library functions.
  /// @{
  /// @brief Generate LLVM IR for the `malloc(int64_t)` C standard library
  /// function declaration.
  void GenerateMallocFunctionDeclaration();
  /// @brief Generate LLVM IR for the `free(void *)` C standard library function
  /// declaration.
  void GenerateFreeFunctionDeclaration();
  /// @brief Generate LLVM IR for the `memset(void *, char, int64_t)` C standard
  /// library function declaration.
  void GenerateMemsetFunctionDeclaration();
  /// @brief Generate LLVM IR for the `printf(const char *, ...)` C standard
  /// library function delcaration.
  void GeneratePrintfFunctionDeclaration();
  /// @brief Generate LLVM IR for the `exit(int)` C standard library function
  /// declaration.
  void GenerateExitFunctionDeclaration();
  /// @}

  /// Generate LLVM IR for the `PunktArray` struct. This struct is
  /// equivalent to the following C definition:
  ///
  /// `
  /// typedef struct {
  ///   int64_t size;
  ///   void *data;
  /// } PunktArray;
  /// `
  void GeneratePunktArrayType();

  /// Printing helper methods.
  /// @{
  void PrintValue(Type *type, llvm::Value *value);
  void PrintBaseTypeValue(BaseType *base_type, llvm::Value *value);
  void PrintArrayTypeValue(ArrayType *array_type, llvm::Value *arr_value);
  void PrintLambdaTypeValue(LambdaType *lambda_type);
  void PrintLineFeed();
  llvm::Value *GetPrintfFormatStringForBaseType(BaseType *base_type);
  /// @}

  CodegenContext *codegen_context;
  std::map<std::string, llvm::Value *> string_map;
};

#endif  // CODE_GENERATION_VISITOR_H_
