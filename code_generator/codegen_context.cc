#include "codegen_context.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <logging/punkt_logger.h>

#include <memory>
#include <string>

std::unique_ptr<CodegenContext> CodegenContext::instance = nullptr;

void CodegenContext::Initialize(std::string module_id) {
  if (instance) {
    // Initializing CodegenContext twice is invalid.
    PunktLogger::LogFatalInternalError(
        "redundant CodegenContext initialization");
  }
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
  // Initialize all targets for emitting object code.
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  auto target_triple = llvm::sys::getDefaultTargetTriple();

  // Initialize a target and check for errors.
  std::string error;
  const llvm::Target *target =
      llvm::TargetRegistry::lookupTarget(target_triple, error);
  if (!target) {
    PunktLogger::LogFatalInternalError(error);
  }

  // Create a generic CPU without any additional feature or target options.
  auto cpu = "generic";
  auto features = "";
  llvm::TargetOptions opt;
  llvm::TargetMachine *target_machine = target->createTargetMachine(
      target_triple, cpu, features, opt, llvm::Reloc::PIC_);

  // Configure the Module.
  module->setDataLayout(target_machine->createDataLayout());
  module->setTargetTriple(target_triple);
}
