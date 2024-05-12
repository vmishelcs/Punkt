#include "codegen_context.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/TargetParser/Host.h>
#include <logging/punkt_logger.h>

#include <memory>
#include <string>

std::unique_ptr<CodegenContext> CodegenContext::instance = nullptr;

void CodegenContext::Initialize(std::string module_id) {
  instance = std::make_unique<CodegenContext>(module_id);
}

CodegenContext &CodegenContext::Get() {
  if (instance == nullptr) {
    PunktLogger::LogFatalInternalError("CodegenContext not initialized");
  }
  return *instance;
}

CodegenContext::CodegenContext(std::string module_id)
    : llvm_context(std::make_unique<llvm::LLVMContext>()),
      module(std::make_unique<llvm::Module>(module_id, *llvm_context)),
      builder(std::make_unique<llvm::IRBuilder<> >(*llvm_context)) {
  std::string target_triple = llvm::sys::getDefaultTargetTriple();
  module->setTargetTriple(target_triple);
}
