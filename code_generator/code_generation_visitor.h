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

class CodeGenerationVisitor : public ParseNodeIRVisitor {
 public:
  CodeGenerationVisitor(std::string module_id);

  void WriteIRToFD(int fd);

  virtual llvm::Value *GenerateCode(ArrayTypeNode &node) override;
  virtual llvm::Value *GenerateCode(CallStatementNode &node) override;
  virtual llvm::Value *GenerateCode(CodeBlockNode &node) override;
  virtual llvm::Value *GenerateCode(DeclarationStatementNode &node) override;
  virtual llvm::Value *GenerateCode(ExpressionStatementNode &node) override;
  virtual llvm::Value *GenerateCode(ForStatementNode &node) override;
  virtual llvm::Value *GenerateCode(FunctionDefinitionNode &node) override;
  virtual llvm::Value *GenerateCode(IfStatementNode &node) override;
  virtual llvm::Value *GenerateCode(LambdaInvocationNode &node) override;
  virtual llvm::Value *GenerateCode(LambdaNode &node) override;
  virtual llvm::Value *GenerateCode(LambdaParameterNode &node) override;
  virtual llvm::Value *GenerateCode(LambdaTypeNode &node) override;
  virtual llvm::Value *GenerateCode(MainNode &node) override;
  virtual llvm::Value *GenerateCode(OperatorNode &node) override;
  virtual llvm::Value *GenerateCode(PrintStatementNode &node) override;
  virtual llvm::Value *GenerateCode(ProgramNode &node) override;
  virtual llvm::Value *GenerateCode(ReturnStatementNode &node) override;
  virtual llvm::Value *GenerateCode(WhileStatementNode &node) override;

  virtual llvm::Value *GenerateCode(ErrorNode &node) override;
  virtual llvm::Value *GenerateCode(NopNode &node) override;
  virtual llvm::Value *GenerateCode(IdentifierNode &node) override;
  virtual llvm::Value *GenerateCode(BooleanLiteralNode &node) override;
  virtual llvm::Value *GenerateCode(CharacterLiteralNode &node) override;
  virtual llvm::Value *GenerateCode(IntegerLiteralNode &node) override;
  virtual llvm::Value *GenerateCode(StringLiteralNode &node) override;
  virtual llvm::Value *GenerateCode(BaseTypeNode &node) override;

  const std::string &GetPunktArrayStructName() const;
  const std::string &GetAllocPunktArrayFunctionName() const;
  const std::string &GetDeallocPunktArrayFunctionName() const;

 private:
  void GenerateGlobalConstants();

  bool IsPreviousInstructionBlockTerminator();

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

  llvm::Value *GetOrCreateString(const std::string &str);

  /// Helpful C standard library functions.
  /// @{
  /// @brief Generate LLVM IR for the `malloc(int)` C standard library function
  /// declaration.
  void GenerateMallocFunctionDeclaration();
  /// @brief Generate LLVM IR for the `free(void *)` C standard library function
  /// declaration.
  void GenerateFreeFunctionDeclaration();
  /// @brief Generate LLVM IR for the `memset(void *, char, int)` C standard
  /// library function declaration.
  void GenerateMemsetFunctionDeclaration();
  /// @brief Generate LLVM for the `printf(const char *, ...)` C standard
  /// library function delcaration.
  void GeneratePrintfFunctionDeclaration();
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

  /// Generate helper functions in LLVM IR for allocating and deallocating Punkt
  /// arrays.
  /// @{
  void GenerateAllocPunktArrayFunction();
  void GenerateDeallocPunktArrayFunction();
  /// @}

  /// Printing helper methods.
  /// @{
  void PrintValue(Type *type, llvm::Value *value);
  void PrintBaseTypeValue(BaseType *base_type, llvm::Value *value);
  void PrintArrayTypeValue(ArrayType *array_type, llvm::Value *arr_value);
  void PrintLambdaTypeValue(LambdaType *lambda_type);
  void PrintLineFeed();
  llvm::Value *GetPrintfFormatStringForBaseType(BaseType *base_type);
  /// @}

  llvm::Value *CodeGenerationInternalError(std::string error_msg);

  std::map<std::string, llvm::Value *> global_constants_table;
  std::map<std::string, llvm::Value *> string_map;
};

#endif  // CODE_GENERATION_VISITOR_H_
