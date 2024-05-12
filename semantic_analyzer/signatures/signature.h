#ifndef SIGNATURE_H_
#define SIGNATURE_H_

#include <parse_node/parse_nodes/operator_node.h>
#include <semantic_analyzer/types/type.h>

#include <utility>
#include <vector>

class CodeGenerationVisitor;

using codegen_function_type = llvm::Value *(*)(CodeGenerationVisitor &,
                                               OperatorNode &);

class Signature {
 public:
  /// @brief Signature constructor for code generator functions requiring 1
  /// `llvm::Value *` argument
  /// @param input_types List of `Type` pointers specifying signature input
  /// types.
  /// @param output_type `Type` pointer specifying signature output type.
  /// @param codegen_function Function pointer to a code generation function
  /// taking an `OperatorNode` as argument.
  Signature(std::initializer_list<Type *> input_types, Type *output_type,
            codegen_function_type codegen_function);

  std::vector<Type *> GetInputTypes() const;
  Type *GetOutputType() const;

  // Returns a pointer to a code generator function.
  codegen_function_type GetCodegenFunction() const;

  /// @brief Checks if this signature accepts the specified types as inputs.
  /// @param types A vector of `Type` pointers representing Punkt types.
  /// @return `true` if this signature accepts the specified types, `false`
  /// otherwise.
  bool Accepts(const std::vector<Type *> &types);

 private:
  /// @brief Reset any `ArbitraryType` objects that this signature may hold to
  /// have no set type.
  void ResetArbitraryTypes();

  std::vector<Type *> input_types;
  Type *output_type;

  // This field holds a function pointer of code generator function (e.g. a
  // function that generates integer addition code) to be used during IR
  // generation stage.
  codegen_function_type codegen_function;
};

#endif  // SIGNATURE_H_
