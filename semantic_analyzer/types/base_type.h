#ifndef BASE_TYPE_H_
#define BASE_TYPE_H_

#include <memory>
#include <string>

#include "type.h"

enum class BaseTypeEnum {
    NULL_TYPE,
    BOOLEAN,
    CHARACTER,
    INTEGER,
    STRING,
    ERROR,
};

/// @brief This class describes all Punkt base types. The Punkt base types are null, boolean,
/// character, integer, string, and error.
class BaseType : public Type {
public:
    /// @brief Create a `BaseType` object specified by a `BaseTypeEnum`.
    /// @param base_type_enum Enum describing the type.
    /// @return `unique_ptr` containing the `BaseType` object.
    static std::unique_ptr<BaseType> Create(BaseTypeEnum base_type_enum);

    /// @brief Create a `unique_ptr<BaseType>` pointing to a `BaseType` object that represents a
    /// null type.
    /// @return `unique_ptr<BaseType>` representing null type.
    static std::unique_ptr<BaseType> CreateNullType();

    /// @brief Create a `unique_ptr<BaseType>` pointing to a `BaseType` object that represents a
    /// boolean type.
    /// @return `unique_ptr<BaseType>` representing boolean type.
    static std::unique_ptr<BaseType> CreateBooleanType();

    /// @brief Create a `unique_ptr<BaseType>` pointing to a `BaseType` object that represents a
    /// character type.
    /// @return `unique_ptr<BaseType>` representing character type.
    static std::unique_ptr<BaseType> CreateCharacterType();

    /// @brief Create a `unique_ptr<BaseType>` pointing to a `BaseType` object that represents an
    /// integer type.
    /// @return `unique_ptr<BaseType>` representing integer type.
    static std::unique_ptr<BaseType> CreateIntegerType();

    /// @brief Create a `unique_ptr<BaseType>` pointing to a `BaseType` object that represents a
    /// string type.
    /// @return `unique_ptr<BaseType>` representing string type.
    static std::unique_ptr<BaseType> CreateStringType();

    /// @brief Create a `unique_ptr<BaseType>` pointing to a `BaseType` object that represents an
    /// error type.
    /// @return `unique_ptr<BaseType>` representing error type.
    static std::unique_ptr<BaseType> CreateErrorType();

    /// @brief Returns the `BaseTypeEnum` associated with this `BaseType`.
    /// @return `BaseTypeEnum` enum class.
    BaseTypeEnum GetBaseTypeEnum() const { return base_type_enum; }

    /// @brief Non-overridden version of `IsEquivalentTo`. Checks if this `BaseType` is equivalent
    ///        to the specified `BaseTypeEnum`.
    /// @param type_enum `BaseTypeEnum` to be checked for semantic equality.
    /// @return `true` if this type is semantically equivalent to the type described by the
    ///         specified `BaseTypeEnum`, `false` otherwise.
    bool IsEquivalentTo(BaseTypeEnum base_type_enum) const;

    /// @brief Checks if this `BaseType` is equivalent to the `Type` pointed to by `other_type`.
    /// @param other_type `Type` pointer.
    /// @return `true` if this type is semantically equivalent to the type pointed to by 
    ///         `other_type`, `false` otherwise.
    virtual bool IsEquivalentTo(Type *other_type) const override;

    /// @brief Creates a `unique_ptr` to a `BaseType` object that is semantically equivalent to
    ///        `this`.
    /// @return `unique_ptr<Type>` that points to a `BaseType` object.
    virtual std::unique_ptr<Type> CreateEquivalentType() const override;


    virtual std::string ToString() const override { return GetEnumString(base_type_enum); };

    virtual bool IsErrorType() const override { return base_type_enum == BaseTypeEnum::ERROR; };

    BaseType(BaseTypeEnum base_type_enum);
    static std::string GetEnumString(BaseTypeEnum base_type_enum);

private:
    BaseTypeEnum base_type_enum;
};

#endif // BASE_TYPE_H_
