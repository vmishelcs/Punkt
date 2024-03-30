#ifndef TYPE_H_
#define TYPE_H_

#include <string>

enum class TypeEnum {
    NO_TYPE,
    CHARACTER,
    INTEGER,
    STRING,
    ERROR
};

struct type_enum_hash {
    std::size_t operator()(TypeEnum t) const {
        return static_cast<std::size_t>(t);
    }
};

class Type {
public:
    Type(TypeEnum type_enum);
    Type(const Type& type);
    Type(Type&& type);

    TypeEnum GetTypeEnum() const;
    std::string ToString() const;
    static std::string GetTypeEnumString(TypeEnum type_enum);
    
    bool IsErrorType() const;

    Type& operator=(Type& other);
    Type& operator=(Type&& other);
    bool operator==(const Type& other) const;

private:
    TypeEnum type_enum;
};

#endif // TYPE_H_
