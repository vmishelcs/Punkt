#ifndef PRIMITIVE_TYPE_H_
#define PRIMITIVE_TYPE_H_

#include "type.h"

enum class PrimitiveTypeEnum {
    NO_TYPE,
    INTEGER,
    ERROR
};

class PrimitiveType : public Type {
public:
    PrimitiveType(PrimitiveTypeEnum type_enum);

    PrimitiveTypeEnum GetTypeEnum() const;

    virtual std::string GetTypeString() const override;

private:
    PrimitiveTypeEnum type_enum;
};

#endif // PRIMITIVE_TYPE_H_
