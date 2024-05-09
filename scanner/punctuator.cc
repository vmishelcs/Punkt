#include "punctuator.h"

#include <logging/punkt_logger.h>

#include <map>
#include <set>

static std::map<std::string, Punctuator> dictionary = {
    {"{", Punctuator::OPEN_BRACE},
    {"}", Punctuator::CLOSE_BRACE},
    {"[", Punctuator::OPEN_BRACKET},
    {"]", Punctuator::CLOSE_BRACKET},
    {"(", Punctuator::OPEN_PARENTHESIS},
    {")", Punctuator::CLOSE_PARENTHESIS},
    {",", Punctuator::SEPARATOR},
    {".", Punctuator::TERMINATOR},
    {"->", Punctuator::ARROW},
    // Operators
    {"[]", Punctuator::ARRAY_IDX},
    {"+", Punctuator::PLUS},
    {"-", Punctuator::MINUS},
    {"*", Punctuator::MUL},
    {"/", Punctuator::DIV},
    {"%", Punctuator::MOD},
    {"==", Punctuator::CMP_EQ},
    {"!=", Punctuator::CMP_NEQ},
    {">", Punctuator::CMP_GT},
    {"<", Punctuator::CMP_LT},
    {">=", Punctuator::CMP_GEQ},
    {"=>", Punctuator::CMP_GEQ},
    {"<=", Punctuator::CMP_LEQ},
    {"=<", Punctuator::CMP_LEQ},
    {"&&", Punctuator::BOOL_AND},
    {"||", Punctuator::BOOL_OR},
    {"!", Punctuator::BOOL_NOT},
    {"=", Punctuator::ASSIGN},
    {"+=", Punctuator::ADD_ASSIGN},
    {"-=", Punctuator::SUB_ASSIGN},
    {"*=", Punctuator::MUL_ASSIGN},
    {"/=", Punctuator::DIV_ASSIGN},
    {"%=", Punctuator::MOD_ASSIGN},
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

Punctuator punctuator_utils::GetPunctuatorEnum(const std::string &lexeme) {
  if (!dictionary.contains(lexeme)) {
    PunktLogger::LogFatalInternalError("no entry for \'" + lexeme +
                                       "\' in punctuator dictionary");
  }
  return dictionary[lexeme];
}

std::string punctuator_utils::GetPunctuatorLexeme(Punctuator punctuator) {
  return reverse_dictionary[punctuator];
}
