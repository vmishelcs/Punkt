#include "punctuator.h"

#include <logging/punkt_logger.h>

#include <map>
#include <set>

static std::map<std::string, Punctuator> dictionary = {
    {"{", Punctuator::OPEN_BRACE},       {"}", Punctuator::CLOSE_BRACE},
    {"[", Punctuator::OPEN_BRACKET},     {"]", Punctuator::CLOSE_BRACKET},
    {"(", Punctuator::OPEN_PARENTHESIS}, {")", Punctuator::CLOSE_PARENTHESIS},
    {",", Punctuator::SEPARATOR},        {".", Punctuator::TERMINATOR},
    {"->", Punctuator::ARROW},
};

static std::map<Punctuator, std::string> reverse_dictionary = []() {
  std::map<Punctuator, std::string> result;
  for (const auto &[key, value] : dictionary) {
    result[value] = key;
  }
  return result;
}();

static std::set<char> punctuator_first_chars = []() {
  std::set<char> result;
  for (const auto &entry : dictionary) {
    result.insert(entry.first[0]);
  }
  return result;
}();

bool punctuator_utils::StartsPunctuator(char ch) {
  return punctuator_first_chars.contains(ch);
}

bool punctuator_utils::IsPunctuator(const std::string &buffer) {
  return dictionary.contains(buffer);
}

Punctuator punctuator_utils::GetPunctuatorEnum(const std::string &lexeme) {
  if (!dictionary.contains(lexeme)) {
    PunktLogger::LogFatalInternalError("no entry for \'" + lexeme +
                                       "\' in punctuator dictionary");
  }
  return dictionary[lexeme];
}

const std::string &punctuator_utils::GetPunctuatorLexeme(
    Punctuator punctuator) {
  return reverse_dictionary[punctuator];
}
