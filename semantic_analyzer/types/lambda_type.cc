#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

#include "lambda_type.h"

std::unique_ptr<LambdaType> LambdaType::CreateLambdaType(
        std::vector<Type *> parameter_types, Type *return_type)
{
    return std::make_unique<LambdaType>(parameter_types, return_type);
}

LambdaType::LambdaType(std::vector<Type *> parameter_types, Type *return_type)
    : Type(TypeEnum::LAMBDA)
{
    for (const auto& param_type : parameter_types) {
        this->parameter_types.push_back(param_type->CreateEquivalentType());
    }
    this->return_type = return_type->CreateEquivalentType();
}

std::vector<Type *> LambdaType::GetParameterTypes() const {
    std::vector<Type *> result;
    if (NumParameters() == 0) {
        return result;
    }

    result.reserve(NumParameters());
    std::transform(parameter_types.begin(), parameter_types.end(),
            std::inserter(result, result.end()),
            [](const auto& param_type) { return param_type.get(); });
    return result;
}

bool LambdaType::IsEquivalentTo(Type *other_type) const {
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
    if (!this->return_type->IsEquivalentTo(other_lambda_type->return_type.get())) {
        return false;
    }

    return true;
}

std::unique_ptr<Type> LambdaType::CreateEquivalentType() const {
    return CreateLambdaType(GetParameterTypes(), return_type.get());
}

std::string LambdaType::ToString() const {
    std::string result = "<";
    for (const auto& param_type : parameter_types) {
        result += (param_type->ToString());
        result += ", ";
    }

    if (parameter_types.size() > 0) {
        // Pop the extra ',' and ' ' characters.
        result.pop_back();
        result.pop_back();
    }
    

    result += "> -> ";
    result += return_type->ToString();
    
    return result;
}
