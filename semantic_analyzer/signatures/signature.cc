#include "signature.h"

#include <code_generator/code_generation_visitor.h>
#include <semantic_analyzer/types/arbitrary_type.h>
#include <semantic_analyzer/types/type.h>

#include <functional>
#include <vector>

using code_gen_function_variant =
    std::variant<llvm::Value *(*)(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *, llvm::Value *),
                 llvm::Value *(*)(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *, llvm::Value *,
                                  llvm::Value *)>;

Signature::Signature(std::initializer_list<Type *> input_types,
                     Type *output_type,
                     llvm::Value *(*fp)(llvm::LLVMContext *context,
                                        llvm::IRBuilder<> *, llvm::Value *))
    : input_types(input_types), output_type(output_type), func_variant(fp) {}

Signature::Signature(std::initializer_list<Type *> input_types,
                     Type *output_type,
                     llvm::Value *(*fp)(llvm::LLVMContext *context,
                                        llvm::IRBuilder<> *, llvm::Value *,
                                        llvm::Value *))
    : input_types(input_types), output_type(output_type), func_variant(fp) {}

std::vector<Type *> Signature::GetInputTypes() const {
  std::vector<Type *> result;
  for (unsigned i = 0, n = input_types.size(); i < n; ++i) {
    result.push_back(input_types[i]);
  }
  return result;
}

Type *Signature::GetOutputType() const { return output_type; }

code_gen_function_variant Signature::GetCodeGenFunc() const {
  return func_variant;
}

bool Signature::Accepts(const std::vector<Type *> &types) {
  // Reset any previously set arbitrary types.
  ResetArbitraryTypes();

  if (types.size() != input_types.size()) {
    return false;
  }

  for (int i = 0, n = input_types.size(); i < n; ++i) {
    if (!(input_types[i]->IsEquivalentTo(types[i]))) {
      return false;
    }
  }

  return true;
}

void Signature::ResetArbitraryTypes() {
  // Reset any arbitrary types that are stored as input types.
  for (Type *type : input_types) {
    if (auto arbitrary_type = dynamic_cast<ArbitraryType *>(type)) {
      arbitrary_type->ResetSetType();
    }
  }

  // Reset output type if it is an arbitrary type.
  auto arbitrary_type = dynamic_cast<ArbitraryType *>(output_type);
  if (arbitrary_type) {
    arbitrary_type->ResetSetType();
  }
}
