#ifndef OPERATOR_H_
#define OPERATOR_H_

#include <string>

enum class Operator {
  ADD_ASSIGN,
  ALLOC,
  ARRAY_IDX,
  ASSIGN,
  BOOL_AND,
  BOOL_NOT,
  BOOL_OR,
  CMP_EQ,
  CMP_GEQ,
  CMP_GT,
  CMP_LEQ,
  CMP_LT,
  CMP_NEQ,
  DIV_ASSIGN,
  DIV,
  MINUS,
  MOD_ASSIGN,
  MOD,
  MUL_ASSIGN,
  MUL,
  OVER,
  PLUS,
  SIZEOF,
  SUB_ASSIGN,
};

namespace operator_utils {

/// @brief Checks if the provided character starts an operator.
/// @param ch `char` to check if it starts an operator.
/// @return `true` if `ch` starts an operator, `false` otherwise.
bool StartsOperator(char ch);

/// @brief Checks if the provided string represents an operator.
/// @param buffer String to check if it is an operator.
/// @return `true` if the specified string represents a operator, `false`
/// otherwise.
bool IsOperator(const std::string &buffer);

/// @brief Get a `Operator` enum corresponding to the provided string.
/// @param buffer String representation of an operator.
/// @return `Operator` enum representation of the operator.
Operator GetOperatorEnum(const std::string &buffer);

/// @brief Get a string representation of the provided operator.
/// @param punctuator_enum `Operator` enum representation of an operator.
/// @return String representation of the operator.
const std::string &GetOperatorLexeme(Operator op);

};  // namespace operator_utils

#endif  // OPERATOR_H_
