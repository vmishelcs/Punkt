#include <memory>
#include <utility>
#include <vector>

#include "lambda_type.h"

std::unique_ptr<LambdaType> LambdaType::CreateLambdaType(
        std::initializer_list<Type *> parameter_types, Type *return_type)
{
    return std::make_unique<LambdaType>(parameter_types, return_type);
}

LambdaType::LambdaType(std::initializer_list<Type *> parameter_types, Type *return_type)
    : Type(TypeEnum::LAMBDA)
{
    
}
