#include <logging/punkt_logger.h>

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
    return "[Type: " + GetTypeEnumString(type_enum) + "]";
}

std::string Type::GetAttributeString() const {
    return GetTypeEnumString(type_enum);
}

std::string Type::GetTypeEnumString(TypeEnum type_enum) {
    switch (type_enum) {
        case TypeEnum::NO_TYPE:
            return "no type";
        case TypeEnum::BOOLEAN:
            return "boolean";
        case TypeEnum::CHARACTER:
            return "character";
        case TypeEnum::INTEGER:
            return "integer";
        case TypeEnum::STRING:
            return "string";
        case TypeEnum::ERROR:
            return "error";
        default:
            return (const char *)PunktLogger::LogFatalInternalError(
                    "unimplemented TypeEnum in Type::GetTypeEnumString");
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

bool Type::operator==(TypeEnum type_enum) const {
    return this->type_enum == type_enum;
}
