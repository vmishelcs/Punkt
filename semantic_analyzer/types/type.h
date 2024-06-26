#ifndef TYPE_H_
#define TYPE_H_

#include <llvm/IR/Type.h>

#include <memory>
#include <string>

enum class TypeEnum { BASE_TYPE, ARRAY, LAMBDA, ARBITRARY_TYPE };

class Type {
 public:
  /// @brief Creates a `unique_ptr` to a semantically equivalent `Type` object
  /// as this `Type`.
  /// @return A semantically equivalent `Type` object as `this`.
  virtual std::unique_ptr<Type> CreateEquivalentType() const = 0;

  /// @brief Check if this `Type` is semantically equivalent to the `Type`
  /// pointed to by `other_type`.
  /// @param type_enum `Type` pointer to be checked for semantic equality.
  /// @return `true` if this type is semantically equivalent to the `Type`
  /// specified by the pointer `other_type`, `false` otherwise.
  virtual bool IsEquivalentTo(Type *other_type) = 0;

  /// @brief Get the `TypeEnum` of this `Type` object.
  /// @return `TypeEnum` of this `Type`.
  TypeEnum GetTypeEnum() const { return type_enum; }

  virtual std::string ToString() const = 0;

  virtual bool IsErrorType() const = 0;

  /// @brief Resets any arbitrary types that make up this type. If this method
  /// is called on an ArbitraryType object, the ArbitraryType is reset. If this
  /// method is called on an ArrayType object that holds an ArbitraryType, the
  /// underlying ArbitraryType is reset. If this method is called on a BaseType,
  /// nothing happens.
  virtual void ResetArbitraryTypes() = 0;

  virtual unsigned GetSizeInBytes() const = 0;
  virtual llvm::Type *GetLLVMType(llvm::LLVMContext &llvm_context) const = 0;

 protected:
  Type(TypeEnum type_enum) : type_enum(type_enum) {}

  TypeEnum type_enum;
};

#endif  // TYPE_H_
