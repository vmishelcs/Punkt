#include "type.h"

Type::Type()
    : type_enum(TypeEnum::NO_TYPE)
{}

Type::Type(TypeEnum type_enum)
    : type_enum(type_enum)
{}

Type::Type(const Type& type)
    : type_enum(type.type_enum)
{}

TypeEnum Type::GetTypeEnum() const {
    return type_enum;
}

std::string Type::GetTypeString() const {
    std::string result = "[Type: ";
    switch (type_enum) {
        case TypeEnum::NO_TYPE:
            result.append("NO TYPE");
            break;
        case TypeEnum::INTEGER:
            result.append("INTEGER");
            break;
        case TypeEnum::ERROR:
            result.append("ERROR");
            break;
        default:
            result.append("UNIMPLEMENTED TYPE");
    }
    result.push_back(']');
    return result;
}

Type& Type::operator=(Type& other) {
    this->type_enum = other.type_enum;
    return *this;
}

Type& Type::operator=(Type&& other) {
    this->type_enum = std::move(other.type_enum);
    return *this;
}
