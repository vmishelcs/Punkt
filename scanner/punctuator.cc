#include "punctuator.h"

std::unordered_map<std::string, PunctuatorEnum> Punctuator::dictionary = {
    { "{", PunctuatorEnum::OPEN_BRACE },
    { "}", PunctuatorEnum::CLOSE_BRACE },
    { ".", PunctuatorEnum::TERMINATOR },
    { "=", PunctuatorEnum::EQUAL },
    { "+", PunctuatorEnum::PLUS },
    { "-", PunctuatorEnum::MINUS },
    { "*", PunctuatorEnum::MULTIPLY },
    { "/", PunctuatorEnum::DIVIDE }
};

Punctuator::Punctuator(std::string lexeme) : ReservedComponent(lexeme) {
    this->punctuator_enum = dictionary.at(lexeme);
}

Punctuator::Punctuator(Punctuator&& punctuator) : ReservedComponent(std::move(punctuator)) {
    this->punctuator_enum = std::move(punctuator.punctuator_enum);
}

bool Punctuator::IsPunctuator(std::string buffer) {
    return dictionary.contains(buffer);
}

int Punctuator::PunctuatorsWithPrefix(std::string prefix) {
    int result = 0;
    for (const auto& entry : dictionary) {
        if (entry.first.starts_with(prefix)) {
            ++result;
        }
    }
    return result;
}

PunctuatorEnum Punctuator::ForLexeme(std::string lexeme) {
    return dictionary.at(lexeme);
}
