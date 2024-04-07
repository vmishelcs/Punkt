#include <functional>
#include <vector>

#include <code_generator/code_generation_visitor.h>
#include <semantic_analyzer/type.h>

#include "signature.h"

using code_gen_function_variant = std::variant<
        llvm::Value *(*)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *),
        llvm::Value *(*)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *, llvm::Value *)>;

Signature::Signature(std::initializer_list<Type> input_types, Type output_type, llvm::Value *(*fp)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *))
    : input_types(input_types)
    , output_type(std::move(output_type))
    , func_variant(fp)
{}

Signature::Signature(std::initializer_list<Type> input_types, Type output_type, llvm::Value *(*fp)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *, llvm::Value *))
    : input_types(input_types)
    , output_type(std::move(output_type))
    , func_variant(fp)
{}

const Type& Signature::GetOutputType() const {
    return output_type;
}

code_gen_function_variant Signature::GetCodeGenFunc() const {
    return func_variant;
}

bool Signature::Accepts(std::vector<Type *>& types) const {
    if (types.size() != input_types.size()) {
        return false;
    }

    for (int i = 0, n = input_types.size(); i < n; ++i) {
        if (!(types[i]->EquivalentTo(input_types[i]))) {
            return false;
        }
    }
    return true;
}
