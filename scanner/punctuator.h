#ifndef PUNCTUATOR_H_
#define PUNCTUATOR_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "reserved_component.h"

enum class PunctuatorEnum {
  OPEN_BRACE,
  CLOSE_BRACE,
  OPEN_PARENTHESIS,
  CLOSE_PARENTHESIS,
  SEPARATOR,
  TERMINATOR,
  EQUAL,
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE,
  CMP_EQ,
  CMP_NEQ,
  CMP_G,
  CMP_L,
  CMP_GEQ,
  CMP_LEQ,
  LANGLE,
  RANGLE,
  ARROW,
};

class Punctuator : public ReservedComponent {
 public:
  Punctuator(std::string lexeme);
  Punctuator(Punctuator &&punctuator);

  PunctuatorEnum GetPunctuatorEnum() const;

  static bool IsPunctuator(std::string buffer);
  static bool IsPunctuatorChar(char c);
  static int PunctuatorsWithPrefix(std::string prefix);
  static PunctuatorEnum ForLexeme(std::string buffer);
  static std::string ForPunctuatorEnum(PunctuatorEnum punctuator_enum);

 private:
  static std::vector<std::string> GetAllPrefixesForPunctuator(
      std::string punctuator_lexeme);

  static std::unordered_map<std::string, PunctuatorEnum> dictionary;
  static std::unordered_map<PunctuatorEnum, std::string> reverse_dictionary;
  static std::unordered_map<std::string, int> num_punctuators_with_prefix;

  PunctuatorEnum punctuator_enum;
};

#endif  // PUNCTUATOR_H_
