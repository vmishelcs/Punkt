#ifndef ARBITRARY_TYPE_H_
#define ARBITRARY_TYPE_H_

#include <memory>
#include <string>

#include "type.h"

/// @brief This class represent an arbitrary Punkt type. The purpose of this
/// class is to be used as a sort of variable when checking if an operator
/// accepts a set of input types.
/// For example, the assignment operator `=` should have the same types on the
/// left and right hand side. Instead of creating a signature for every possible
/// Punkt type, we can say that the `=` operator must have the signature of
/// `(ArbitraryType T, ArbitraryType T) -> ArbitraryType T`, where `T` denotes
/// some Punkt type.
class ArbitraryType : public Type {
 public:
  ArbitraryType() : Type(TypeEnum::ARBITRARY_TYPE), set_type(nullptr) {}

  /// This method should probably never be called for `ArbitraryType` objects.
  virtual std::unique_ptr<Type> CreateEquivalentType() const override;

  /// @brief Checks if another type is equivalent to the set type of this
  /// arbitrary type. Note that if set type has not been set, it is set to the
  /// specified type.
  /// @param other_type Type to check for semantic equivalence.
  /// @return `true` if this type is semantically equivalent to `other_type`, or
  /// if the set type hasn't been established. Otherwise, returns `false`.
  virtual bool IsEquivalentTo(Type *other_type) override;

  virtual std::string ToString() const override { return "placeholder type"; }

  virtual bool IsErrorType() const override { return false; }

  virtual void ResetArbitraryTypes() override;

 private:
  /// @brief Reset the set type on this arbitrary type.
  void ResetSetType() { set_type = nullptr; }

  Type *set_type;
};

#endif  // ARBITRARY_TYPE_H_
