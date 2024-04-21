#ifndef LAMBDA_TYPE_H_
#define LAMBDA_TYPE_H_

#include <memory>
#include <vector>

#include "type.h"

/// @brief This class is used to denote Punkt lambda types. Functions and lambda
/// variables are given the lambda type, consisting of a list of parameter
/// types, and a return type. For example, a lambda type of `<integer, bool> ->
/// integer` describes a lambda that takes two parameters (one integer value and
/// one boolean value), and returns an integer value.
class LambdaType : public Type {
 public:
  /// @brief Create a `unique_ptr<LambdaType>` object specified by given
  /// parameter types and return type.
  /// @param parameter_types A `vector` of `Type` pointers describing the
  /// parameters of the `LambdaType`.
  /// @param return_type A pointer to a `Type` object describing the return type
  /// of the `LambdaType`.
  /// @return A `unique_ptr<LambdaType>` object.
  static std::unique_ptr<LambdaType> CreateLambdaType(
      std::vector<Type *> parameter_types, Type *return_type);

  /// @brief Checks that this `LambdaType` accepts the specified types as
  /// arguments.
  /// @param arg_types A `vector` of types to check.
  /// @return `true` if each `Type` specified by the `arg_types` vector is
  /// semantically equivalent to the parameter types specified in this
  /// `LambdaType`.
  bool AcceptsArgumentTypes(std::vector<Type *> &arg_types) const;

  int NumParameters() const { return parameter_types.size(); }

  std::vector<Type *> GetParameterTypes() const;

  Type *GetReturnType() const { return return_type.get(); }

  /// @brief Check if this `LambdaType` is semantically equivalent to the `Type`
  /// pointed to by `other_type`.
  /// @param type_enum `Type` pointer to be checked for semantic equality.
  /// @return `true` if this type is semantically equivalent to the `Type`
  /// specified by the
  ///         pointer `other_type`, `false` otherwise.
  virtual bool IsEquivalentTo(Type *other_type) const override;

  /// @brief Creates a `unique_ptr` to a `Lambda` object that is semantically
  /// equivalent to `this`.
  /// @return `unique_ptr<Type>` that points to a `LambdaType` object.
  virtual std::unique_ptr<Type> CreateEquivalentType() const override;

  virtual std::string ToString() const override;

  virtual bool IsErrorType() const override { return false; }

  LambdaType(std::vector<Type *> parameter_types, Type *return_type);

 private:
  std::vector<std::unique_ptr<Type>> parameter_types;
  std::unique_ptr<Type> return_type;
};

#endif  // LAMBDA_TYPE_H_
