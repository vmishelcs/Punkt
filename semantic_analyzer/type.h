#ifndef TYPE_H_
#define TYPE_H_

#include <string>

enum class TypeEnum {
    NO_TYPE,
    INTEGER,
    ERROR
};

class Type {
public:
    Type();
    Type(TypeEnum type_enum);
    TypeEnum GetTypeEnum() const;
    std::string GetTypeString() const;

    Type& operator=(Type& other);
    Type& operator=(Type&& other);

private:
    TypeEnum type_enum;
};

#endif // TYPE_H_
