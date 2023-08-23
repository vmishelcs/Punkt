#include <stdexcept>

#include "punctuator.h"

std::unordered_map<std::string, PunctuatorEnum> Punctuator::dictionary = {
    { "{", PunctuatorEnum::OPEN_BRACE },
    { "}", PunctuatorEnum::CLOSE_BRACE },
    { "(", PunctuatorEnum::OPEN_PARENTHESIS },
    { ")", PunctuatorEnum::CLOSE_PARENTHESIS },
    { ".", PunctuatorEnum::TERMINATOR },
    { "=", PunctuatorEnum::EQUAL },
    { "+", PunctuatorEnum::PLUS },
    { "-", PunctuatorEnum::MINUS },
    { "*", PunctuatorEnum::MULTIPLY },
    { "/", PunctuatorEnum::DIVIDE }
};

std::unordered_set<char> Punctuator::punctuator_chars = [] {
    std::unordered_set<char> result;
    for (const auto& entry: dictionary) {
        for (char c : entry.first) {
            result.insert(c);
        }
    }
    return result;
}();

std::unordered_map<std::string, int> Punctuator::num_punctuators_with_prefix = [] {
    /**
     * Calculates the number of punctuators for each possible prefix.
     * Inspired by:
     * https://stackoverflow.com/questions/76942081/creating-a-static-unordered-set-from-keys-of-a-static-unordered-map
     */
    std::vector<std::vector<std::string>> prefixes_per_punctuator;
    std::transform(
        dictionary.begin(),
        dictionary.end(),
        std::inserter(prefixes_per_punctuator, prefixes_per_punctuator.end()),
        [](auto&& entry) {
            std::vector<std::string> prefixes = GetAllPrefixesForPunctuator(entry.first);
            return prefixes;
        }
    );

    std::unordered_map<std::string, int> result;
    for (auto prefix_list : prefixes_per_punctuator) {
        for (auto prefix : prefix_list) {
            ++result[prefix];
        }
    }
    return result;
}();

Punctuator::Punctuator(std::string lexeme) : ReservedComponent(lexeme) {
    this->punctuator_enum = dictionary.at(lexeme);
}

Punctuator::Punctuator(Punctuator&& punctuator) : ReservedComponent(std::move(punctuator)) {
    this->punctuator_enum = std::move(punctuator.punctuator_enum);
}

PunctuatorEnum Punctuator::GetPunctuatorEnum() const {
    return punctuator_enum;
}

bool Punctuator::IsPunctuator(std::string buffer) {
    return dictionary.contains(buffer);
}

bool Punctuator::IsPunctuatorChar(char c) {
    return punctuator_chars.contains(c);
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

std::vector<std::string> Punctuator::GetAllPrefixesForPunctuator(std::string punctuator_lexeme) {
    if (!IsPunctuator(punctuator_lexeme)) {
        throw new std::runtime_error(
            "Non-punctuator lexeme provided to GetAllPrefixesForPunctuator"
        );
    }
    
    std::vector<std::string> result;
    int n = punctuator_lexeme.length();
    for (int i = 1; i <= n; ++i) {
        result.emplace_back(punctuator_lexeme.substr(0, i));
    }
    return result;
}
