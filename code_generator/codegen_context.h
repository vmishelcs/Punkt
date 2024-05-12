#ifndef CODEGEN_CONTEXT_H_
#define CODEGEN_CONTEXT_H_

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <string>

/// @brief Code generation context singleton containing fundamental LLVM objects
/// required to generate IR.
class CodegenContext {
 public:
  static void Initialize(std::string module_id);
  static CodegenContext &Get();

  CodegenContext(CodegenContext const &) = delete;
  CodegenContext &operator=(CodegenContext const &) = delete;

  llvm::LLVMContext *GetLLVMContext() const { return llvm_context.get(); }
  llvm::Module *GetModule() const { return module.get(); }
  llvm::IRBuilder<> *GetIRBuilder() const { return builder.get(); }

 private:
  friend std::unique_ptr<CodegenContext> std::make_unique<CodegenContext>(
      std::string &);

  CodegenContext(std::string module_id);

  static std::unique_ptr<CodegenContext> instance;

  std::unique_ptr<llvm::LLVMContext> llvm_context;
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::IRBuilder<>> builder;
};

#endif  // CODEGEN_CONTEXT_H_
