#include "primitive_type.h"

PrimitiveType::PrimitiveType(PrimitiveTypeEnum type_enum) {
    this->type_enum = type_enum;
}

PrimitiveTypeEnum PrimitiveType::GetTypeEnum() const {
    return type_enum;
}

std::string PrimitiveType::GetTypeString() const {
    std::string result = "Type: ";
    switch (type_enum) {
        case PrimitiveTypeEnum::NO_TYPE:
            result.append("NO TYPE");
            break;
        case PrimitiveTypeEnum::INTEGER:
            result.append("INTEGER");
            break;
        case PrimitiveTypeEnum::ERROR:
            result.append("ERROR");
            break;
        default:
            result.append("UNIMPLEMENTED TYPE");
    }
    return result;
}
