#ifndef LAMBDA_TYPE_H_
#define LAMBDA_TYPE_H_

#include <memory>
#include <utility>
#include <vector>

#include "type.h"

class LambdaType : public Type {
public:
    static std::unique_ptr<LambdaType> CreateLambdaType(
            std::initializer_list<Type *> parameter_types,
            Type *return_type);

    LambdaType(std::initializer_list<Type*> parameter_types, Type *return_type);

private:
    std::vector<std::unique_ptr<Type>> parameter_types;
    std::unique_ptr<Type> return_type;
};

#endif // LAMBDA_TYPE_H_
