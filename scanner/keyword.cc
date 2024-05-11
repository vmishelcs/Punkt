#include "keyword.h"

#include <logging/punkt_logger.h>

#include <map>
#include <string>

static std::map<std::string, Keyword> dictionary = {
    {"alloc", Keyword::ALLOC},       {"bool", Keyword::BOOL},
    {"call", Keyword::CALL},         {"char", Keyword::CHAR},
    {"const", Keyword::CONST},       {"else", Keyword::ELSE},
    {"false", Keyword::FALSE},       {"for", Keyword::FOR},
    {"function", Keyword::FUNCTION}, {"if", Keyword::IF},
    {"int", Keyword::INT},           {"main", Keyword::MAIN},
    {"print", Keyword::PRINT},       {"println", Keyword::PRINTLN},
    {"return", Keyword::RETURN},     {"string", Keyword::STRING},
    {"true", Keyword::TRUE},         {"var", Keyword::VAR},
    {"void", Keyword::VOID},         {"while", Keyword::WHILE}};

static std::map<Keyword, std::string> reverse_dictionary = [] {
  std::map<Keyword, std::string> result;
  for (const auto &[key, value] : dictionary) {
    result[value] = key;
  }
  return result;
}();

bool keyword_utils::IsKeyword(const std::string &lexeme) {
  return dictionary.contains(lexeme);
}

Keyword keyword_utils::GetKeywordEnum(const std::string &lexeme) {
  if (!dictionary.contains(lexeme)) {
    PunktLogger::LogFatalInternalError("no entry for \'" + lexeme +
                                       "\' in keyword dictionary");
  }
  return dictionary[lexeme];
}

const std::string &keyword_utils::GetKeywordLexeme(Keyword keyword) {
  return reverse_dictionary[keyword];
}
