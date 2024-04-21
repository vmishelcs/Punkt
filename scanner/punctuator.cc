#include "punctuator.h"

#include <logging/punkt_logger.h>

#include <algorithm>
#include <stdexcept>

std::unordered_map<std::string, PunctuatorEnum> Punctuator::dictionary = {
    {"{", PunctuatorEnum::OPEN_BRACE},
    {"}", PunctuatorEnum::CLOSE_BRACE},
    {"(", PunctuatorEnum::OPEN_PARENTHESIS},
    {")", PunctuatorEnum::CLOSE_PARENTHESIS},
    {",", PunctuatorEnum::SEPARATOR},
    {".", PunctuatorEnum::TERMINATOR},
    {"=", PunctuatorEnum::EQUAL},
    {"+", PunctuatorEnum::PLUS},
    {"-", PunctuatorEnum::MINUS},
    {"*", PunctuatorEnum::MULTIPLY},
    {"/", PunctuatorEnum::DIVIDE},
    {"==", PunctuatorEnum::CMP_EQ},
    {"!=", PunctuatorEnum::CMP_NEQ},
    {">", PunctuatorEnum::CMP_G},
    {"<", PunctuatorEnum::CMP_L},
    {">=", PunctuatorEnum::CMP_GEQ},
    {"=>", PunctuatorEnum::CMP_GEQ},
    {"<=", PunctuatorEnum::CMP_LEQ},
    {"=<", PunctuatorEnum::CMP_LEQ},
    {"->", PunctuatorEnum::ARROW},
};

std::unordered_map<PunctuatorEnum, std::string> Punctuator::reverse_dictionary =
    [] {
      std::unordered_map<PunctuatorEnum, std::string> result;
      for (const auto& [key, value] : dictionary) {
        result[value] = key;
      }
      return result;
    }();

std::unordered_map<std::string, int> Punctuator::num_punctuators_with_prefix =
    [] {
      /**
       * Calculates the number of punctuators for each possible prefix.
       * Inspired by:
       * https://stackoverflow.com/questions/76942081/creating-a-static-unordered-set-from-keys-of-a-static-unordered-map
       */
      std::vector<std::vector<std::string>> prefixes_per_punctuator;
      std::transform(
          dictionary.begin(), dictionary.end(),
          std::inserter(prefixes_per_punctuator, prefixes_per_punctuator.end()),
          [](auto&& entry) {
            std::vector<std::string> prefixes =
                GetAllPrefixesForPunctuator(entry.first);
            return prefixes;
          });

      std::unordered_map<std::string, int> result;
      for (auto prefix_list : prefixes_per_punctuator) {
        for (auto prefix : prefix_list) {
          ++result[prefix];
        }
      }
      return result;
    }();

Punctuator::Punctuator(std::string lexeme) : ReservedComponent(lexeme) {
  auto find_result = dictionary.find(lexeme);
  if (find_result == dictionary.end()) {
    PunktLogger::LogFatalInternalError("lexeme \"" + lexeme +
                                       "\" missing from Punctuator dictionary");
  }
  this->punctuator_enum = find_result->second;
}

Punctuator::Punctuator(Punctuator&& punctuator)
    : ReservedComponent(std::move(punctuator)) {
  this->punctuator_enum = std::move(punctuator.punctuator_enum);
}

PunctuatorEnum Punctuator::GetPunctuatorEnum() const { return punctuator_enum; }

bool Punctuator::IsPunctuator(std::string buffer) {
  return dictionary.contains(buffer);
}

int Punctuator::PunctuatorsWithPrefix(std::string prefix) {
  if (num_punctuators_with_prefix.contains(prefix)) {
    return num_punctuators_with_prefix.at(prefix);
  }
  return 0;
}

PunctuatorEnum Punctuator::ForLexeme(std::string lexeme) {
  return dictionary.at(lexeme);
}

std::string Punctuator::ForPunctuatorEnum(PunctuatorEnum punctuator_enum) {
  return reverse_dictionary.at(punctuator_enum);
}

std::vector<std::string> Punctuator::GetAllPrefixesForPunctuator(
    std::string punctuator_lexeme) {
  if (!IsPunctuator(punctuator_lexeme)) {
    throw new std::runtime_error(
        "Non-punctuator lexeme provided to GetAllPrefixesForPunctuator");
  }

  std::vector<std::string> result;
  int n = punctuator_lexeme.length();
  for (int i = 1; i <= n; ++i) {
    result.emplace_back(punctuator_lexeme.substr(0, i));
  }
  return result;
}
