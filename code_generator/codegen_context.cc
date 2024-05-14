#include "codegen_context.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <logging/punkt_logger.h>

#include <memory>
#include <string>

std::unique_ptr<CodegenContext> CodegenContext::instance = nullptr;

void CodegenContext::Initialize(std::string module_id) {
  instance = std::make_unique<CodegenContext>(module_id);
}

CodegenContext *CodegenContext::Get() {
  if (instance == nullptr) {
    PunktLogger::LogFatalInternalError("CodegenContext not initialized");
  }
  return instance.get();
}

CodegenContext::CodegenContext(std::string module_id)
    : llvm_context(std::make_unique<llvm::LLVMContext>()),
      module(std::make_unique<llvm::Module>(module_id, *llvm_context)),
      builder(std::make_unique<llvm::IRBuilder<> >(*llvm_context)) {
  // TODO: Find a way to avoid hardcoding data layout.
  module->setDataLayout(
      "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128");
  module->setTargetTriple(llvm::sys::getDefaultTargetTriple());
}
