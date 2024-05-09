#ifndef PUNCTUATOR_H_
#define PUNCTUATOR_H_

#include <string>

enum class Punctuator {
  OPEN_BRACE,
  CLOSE_BRACE,
  OPEN_BRACKET,
  CLOSE_BRACKET,
  OPEN_PARENTHESIS,
  CLOSE_PARENTHESIS,
  SEPARATOR,
  TERMINATOR,
  ARROW,
  // Operators
  ALLOC,
  ARRAY_IDX,
  PLUS,
  MINUS,
  MUL,
  DIV,
  MOD,
  CMP_EQ,
  CMP_NEQ,
  CMP_GT,
  CMP_LT,
  CMP_GEQ,
  CMP_LEQ,
  BOOL_AND,
  BOOL_OR,
  BOOL_NOT,
  ASSIGN,
  ADD_ASSIGN,
  SUB_ASSIGN,
  MUL_ASSIGN,
  DIV_ASSIGN,
  MOD_ASSIGN,
};

namespace punctuator_utils {

/// @brief Checks if a provided character starts a punctuator.
/// @param ch `char` to check if it starts a punctuator.
/// @return `true` if `ch` starts a punctuator, `false` otherwise.
bool StartsPunctuator(char ch);

/// @brief Get a `Punctuator` enum corresponding to the provided string.
/// @param lexeme String representation of a punctuator.
/// @return `Punctuator`enum representation of the punctuator.
Punctuator GetPunctuatorEnum(const std::string &lexeme);

/// @brief Get a string representation of the provided punctuator.
/// @param punctuator_enum `Punctuator` enum representation of a punctuator.
/// @return String representation of the punctuator.
std::string GetPunctuatorLexeme(Punctuator punctuator_enum);

};  // namespace punctuator_utils

#endif  // PUNCTUATOR_H_
