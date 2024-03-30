#include "type.h"

Type::Type(TypeEnum type_enum)
    : type_enum(type_enum)
{}

Type::Type(const Type& type)
    : type_enum(type.type_enum)
{}

Type::Type(Type&& type)
    : type_enum(std::move(type.type_enum))
{}

TypeEnum Type::GetTypeEnum() const {
    return type_enum;
}

std::string Type::ToString() const {
    std::string result = "[Type: " + GetTypeEnumString(type_enum) + "]";
    return result;
}

std::string Type::GetTypeEnumString(TypeEnum type_enum) {
    switch (type_enum) {
        case TypeEnum::NO_TYPE:
            return "NO TYPE";
        case TypeEnum::CHARACTER:
            return "CHARACTER";
        case TypeEnum::INTEGER:
            return "INTEGER";
        case TypeEnum::STRING:
            return "STRING";
        case TypeEnum::ERROR:
            return "ERROR";
        default:
            return "UNIMPLEMENTED_TYPE";
    }
}

bool Type::IsErrorType() const {
    return type_enum == TypeEnum::ERROR;
}

Type& Type::operator=(Type& other) {
    this->type_enum = other.type_enum;
    return *this;
}

Type& Type::operator=(Type&& other) {
    this->type_enum = std::move(other.type_enum);
    return *this;
}

bool Type::operator==(const Type& other) const {
    return this->type_enum == other.type_enum;
}
