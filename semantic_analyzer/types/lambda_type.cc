#include "lambda_type.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

static const int kLambdaTypeSizeInBytes = 8;

std::unique_ptr<LambdaType> LambdaType::CreateLambdaType(
    std::vector<Type *> parameter_types, Type *return_type) {
  return std::make_unique<LambdaType>(parameter_types, return_type);
}

bool LambdaType::AcceptsArgumentTypes(std::vector<Type *> &arg_types) const {
  // Make sure the number of arguments is the same.
  if (arg_types.size() != this->parameter_types.size()) {
    return false;
  }

  // Check that each argument is semantically equivalent.
  for (unsigned i = 0, n = arg_types.size(); i < n; ++i) {
    if (!parameter_types[i]->IsEquivalentTo(arg_types[i])) {
      return false;
    }
  }

  return true;
}

std::vector<Type *> LambdaType::GetParameterTypes() const {
  std::vector<Type *> result;
  if (NumParameters() == 0) {
    return result;
  }

  result.reserve(NumParameters());
  std::transform(parameter_types.begin(), parameter_types.end(),
                 std::inserter(result, result.end()),
                 [](const auto &param_type) { return param_type.get(); });
  return result;
}

bool LambdaType::IsEquivalentTo(Type *other_type) {
  LambdaType *other_lambda_type = dynamic_cast<LambdaType *>(other_type);
  if (!other_lambda_type) {
    return false;
  }

  // Check that the number of parameters is the same.
  if (this->NumParameters() != other_lambda_type->NumParameters()) {
    return false;
  }

  // Check that the parameter types are equivalent.
  for (unsigned i = 0, n = NumParameters(); i < n; ++i) {
    if (!this->parameter_types[i]->IsEquivalentTo(
            other_lambda_type->parameter_types[i].get())) {
      return false;
    }
  }

  // Check that the return type is equivalent.
  if (!this->return_type->IsEquivalentTo(
          other_lambda_type->return_type.get())) {
    return false;
  }

  return true;
}

std::unique_ptr<Type> LambdaType::CreateEquivalentType() const {
  return CreateLambdaType(GetParameterTypes(), return_type.get());
}

std::string LambdaType::ToString() const {
  std::string result = "(";
  for (const auto &param_type : parameter_types) {
    if (param_type->GetTypeEnum() == TypeEnum::LAMBDA)
      // Wrap lambda types in brackets for readability.
      result.push_back('(');

    result += param_type->ToString();
    if (param_type->GetTypeEnum() == TypeEnum::LAMBDA)
      // Wrap lambda types in brackets for readability.
      result.push_back(')');

    result += ", ";
  }

  if (parameter_types.size() > 0) {
    // Pop the extra ',' and ' ' characters.
    result.pop_back();
    result.pop_back();
  }

  result += ") -> ";

  if (return_type->GetTypeEnum() == TypeEnum::LAMBDA)
    // Wrap lambda types in brackets for readability.
    result.push_back('(');

  result += return_type->ToString();

  if (return_type->GetTypeEnum() == TypeEnum::LAMBDA)
    // Wrap lambda types in brackets for readability.
    result.push_back(')');

  return result;
}

void LambdaType::ResetArbitraryTypes() {
  for (unsigned i = 0, n = parameter_types.size(); i < n; ++i) {
    parameter_types[i]->ResetArbitraryTypes();
  }
  return_type->ResetArbitraryTypes();
}

int LambdaType::GetSizeInBytes() const {
  // Lambdas are pointers.
  return kLambdaTypeSizeInBytes;
}

llvm::Type *LambdaType::GetLLVMType(llvm::LLVMContext &llvm_context) const {
  return llvm::PointerType::getUnqual(llvm_context);
}

LambdaType::LambdaType(std::vector<Type *> parameter_types_to_copy,
                       Type *return_type)
    : Type(TypeEnum::LAMBDA) {
  for (const auto &param_type : parameter_types_to_copy) {
    this->parameter_types.push_back(param_type->CreateEquivalentType());
  }
  this->return_type = return_type->CreateEquivalentType();
}
