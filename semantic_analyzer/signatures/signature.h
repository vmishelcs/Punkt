#ifndef SIGNATURE_H_
#define SIGNATURE_H_

#include <utility>
#include <variant>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <code_generator/code_generation_visitor.h>
#include <semantic_analyzer/types/type.h>

using code_gen_function_variant = std::variant<
llvm::Value *(*)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *),
llvm::Value *(*)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *, llvm::Value *)>;

class Signature {
public:
    /// @brief Signature constructor for code generator functions requiring 1 `llvm::Value *`
    /// argument
    /// @param input_types List of `Type` pointers specifying signature input types.
    /// @param output_type `Type` pointer specifying signature output type.
    /// @param fp Function pointer to a code generation function with 1 parameter for `llvm::Value`.
    Signature(
        std::initializer_list<Type *> input_types,
        Type *output_type,
        llvm::Value *(*fp)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *));

    /// @brief Signature constructor for code generator functions requiring 2 `llvm::Value *`
    /// argument
    /// @param input_types List of `Type` pointers specifying signature input types.
    /// @param output_type `Type` pointer specifying signature output type.
    /// @param fp Function pointer to a code generation function with 2 parameters for
    /// `llvm::Value *`.
    Signature(
        std::initializer_list<Type *> input_types,
        Type *output_type,
        llvm::Value *(*fp)(llvm::LLVMContext *context, llvm::IRBuilder<> *, llvm::Value *, llvm::Value *));


    std::vector<Type *> GetInputTypes() const;
    Type *GetOutputType() const;

    // Returns a variant to a code generator function.
    code_gen_function_variant GetCodeGenFunc() const;

    bool Accepts(std::vector<Type *>& types) const;

private:
    std::vector<Type *> input_types;
    Type *output_type;

    // This field holds a variant of code generator function (e.g. a function that generates integer
    // addition code) to be used during IR generation stage.
    code_gen_function_variant func_variant;
};

#endif // SIGNATURE_H_
