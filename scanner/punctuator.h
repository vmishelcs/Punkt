#ifndef PUNCTUATOR_H_
#define PUNCTUATOR_H_

#include <string>

enum class Punctuator {
  ARROW,
  CLOSE_BRACE,
  CLOSE_BRACKET,
  CLOSE_PARENTHESIS,
  OPEN_BRACE,
  OPEN_BRACKET,
  OPEN_PARENTHESIS,
  SEPARATOR,
  TERMINATOR,
};

namespace punctuator_utils {

/// @brief Checks if the provided character starts a punctuator.
/// @param ch `char` to check if it starts a punctuator.
/// @return `true` if `ch` starts a punctuator, `false` otherwise.
bool StartsPunctuator(char ch);

/// @brief Checks if the provided string represents a punctuator.
/// @param buffer String to check if it is a punctuator.
/// @return `true` if the specified string represents a punctuator, `false`
/// otherwise.
bool IsPunctuator(const std::string &buffer);

/// @brief Get a `Punctuator` enum corresponding to the provided string.
/// @param lexeme String representation of a punctuator.
/// @return `Punctuator` enum representation of the punctuator.
Punctuator GetPunctuatorEnum(const std::string &lexeme);

/// @brief Get a string representation of the provided punctuator.
/// @param punctuator_enum `Punctuator` enum representation of a punctuator.
/// @return String representation of the punctuator.
const std::string &GetPunctuatorLexeme(Punctuator punctuator_enum);

};  // namespace punctuator_utils

#endif  // PUNCTUATOR_H_
