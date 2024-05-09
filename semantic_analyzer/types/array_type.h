#ifndef ARRAY_TYPE_H_
#define ARRAY_TYPE_H_

#include "type.h"

/// @brief This class is used to denote Punkt array types.
class ArrayType : public Type {
 public:
  /// @brief Create an array type with a specified subtype.
  /// @param subtype The type which this array type holds.
  /// @return A `unique_ptr` holding the array type object.
  static std::unique_ptr<ArrayType> CreateArrayType(Type *subtype);

  virtual std::unique_ptr<Type> CreateEquivalentType() const override;

  virtual bool IsEquivalentTo(Type *other_type) override;

  virtual std::string ToString() const override;

  virtual bool IsErrorType() const override { return false; }

  ArrayType(Type *subtype);

 private:
  std::unique_ptr<Type> subtype;
};

#endif  // ARRAY_TYPE_H_
