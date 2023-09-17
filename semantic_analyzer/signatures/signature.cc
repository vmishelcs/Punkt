#include "signature.h"

Signature::Signature(std::initializer_list<Type> input_types, Type output_type)
    : input_types(input_types)
    , output_type(std::move(output_type))
{}

const Type& Signature::GetOutputType() const {
    return output_type;
}

bool Signature::Accepts(std::vector<std::reference_wrapper<const Type>>& types) const {
    if (types.size() != input_types.size()) {
        return false;
    }

    int n = input_types.size();
    for (int i = 0; i < n; ++i) {
        if (input_types[i] != types[i].get()) {
            return false;
        }
    }
    return true;
}
