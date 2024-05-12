#include "signature.h"

#include <parse_node/parse_nodes/operator_node.h>
#include <semantic_analyzer/types/type.h>

#include <vector>

class CodeGenerationVisitor;

using codegen_function_type = llvm::Value *(*)(CodeGenerationVisitor &,
                                               OperatorNode &);

Signature::Signature(std::initializer_list<Type *> input_types,
                     Type *output_type, codegen_function_type codegen_function)
    : input_types(input_types),
      output_type(output_type),
      codegen_function(codegen_function) {}

std::vector<Type *> Signature::GetInputTypes() const {
  std::vector<Type *> result;
  for (unsigned i = 0, n = input_types.size(); i < n; ++i) {
    result.push_back(input_types[i]);
  }
  return result;
}

Type *Signature::GetOutputType() const { return output_type; }

codegen_function_type Signature::GetCodegenFunction() const {
  return codegen_function;
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
    type->ResetArbitraryTypes();
  }
  output_type->ResetArbitraryTypes();
}
