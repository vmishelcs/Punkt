#include "operator.h"

#include <logging/punkt_logger.h>

#include <map>
#include <set>
#include <string>

static std::map<std::string, Operator> dictionary = {
    {"[]", Operator::ARRAY_IDX},  {"+", Operator::PLUS},
    {"-", Operator::MINUS},       {"*", Operator::MUL},
    {"/", Operator::DIV},         {"%", Operator::MOD},
    {"==", Operator::CMP_EQ},     {"!=", Operator::CMP_NEQ},
    {">", Operator::CMP_GT},      {"<", Operator::CMP_LT},
    {">=", Operator::CMP_GEQ},    {"=>", Operator::CMP_GEQ},
    {"<=", Operator::CMP_LEQ},    {"=<", Operator::CMP_LEQ},
    {"&&", Operator::BOOL_AND},   {"||", Operator::BOOL_OR},
    {"!", Operator::BOOL_NOT},    {"=", Operator::ASSIGN},
    {"+=", Operator::ADD_ASSIGN}, {"-=", Operator::SUB_ASSIGN},
    {"*=", Operator::MUL_ASSIGN}, {"/=", Operator::DIV_ASSIGN},
    {"%=", Operator::MOD_ASSIGN},
};

static std::map<Operator, std::string> reverse_dictionary = []() {
  std::map<Operator, std::string> result;
  for (const auto &[key, value] : dictionary) {
    result[value] = key;
  }
  return result;
}();

static std::set<char> operator_first_chars = []() {
  std::set<char> result;
  for (const auto &entry : dictionary) {
    result.insert(entry.first[0]);
  }
  return result;
}();

bool operator_utils::StartsOperator(char ch) {
  return operator_first_chars.contains(ch);
}

bool operator_utils::IsOperator(const std::string &buffer) {
  return dictionary.contains(buffer);
}

Operator operator_utils::GetOperatorEnum(const std::string &buffer) {
  if (!dictionary.contains(buffer)) {
    PunktLogger::LogFatalInternalError("no entry for \'" + buffer +
                                       "\' in operator dictionary");
  }
  return dictionary[buffer];
}

const std::string &operator_utils::GetOperatorLexeme(Operator op) {
  return reverse_dictionary[op];
}
