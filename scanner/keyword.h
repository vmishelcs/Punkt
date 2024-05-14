#ifndef KEYWORD_H_
#define KEYWORD_H_

#include <string>

enum class Keyword {
  ALLOC,
  BOOL,
  CALL,
  CHAR,
  CONST,
  ELSE,
  FALSE,
  FOR,
  FUNCTION,
  IF,
  INT,
  MAIN,
  PRINT,
  PRINTLN,
  RETURN,
  SIZEOF,
  STRING,
  TRUE,
  VAR,
  VOID,
  WHILE
};

namespace keyword_utils {

/// @brief Checks if the provided lexeme is a keyword.
/// @param lexeme String to be checked if it is a keyword.
/// @return `true` if `lexeme` is a keyword, `false` otherwise.
bool IsKeyword(const std::string& lexeme);

/// @brief Get a `Keyword` enum corresponding to the provided string.
/// @param lexeme String representation of a keyword.
/// @return A `Keyword` enum corresponding to the provided string.
Keyword GetKeywordEnum(const std::string& lexeme);

/// @brief Get a string representation of the provided keyword.
/// @param keyword `Keyword` enum representation of a punctuator.
/// @return String representation of the `keyword`.
const std::string& GetKeywordLexeme(Keyword keyword);

}  // namespace keyword_utils

#endif  // KEYWORD_H_
