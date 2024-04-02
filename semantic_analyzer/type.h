#ifndef TYPE_H_
#define TYPE_H_

#include <string>

enum class TypeEnum {
    NO_TYPE,
    BOOLEAN,
    CHARACTER,
    INTEGER,
    STRING,
    ERROR
};

class Type {
public:
    Type(TypeEnum type_enum);
    Type(const Type& type);
    Type(Type&& type);

    TypeEnum GetTypeEnum() const;
    std::string ToString() const;
    std::string GetAttributeString() const;
    static std::string GetTypeEnumString(TypeEnum type_enum);
    
    bool IsErrorType() const;

    Type& operator=(Type& other);
    Type& operator=(Type&& other);
    bool operator==(const Type& other) const;

private:
    TypeEnum type_enum;
};

#endif // TYPE_H_
