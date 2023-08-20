#include "punctuator.h"

std::unordered_map<std::string, PunctuatorEnum> Punctuator::dictionary = {
    { "{", PunctuatorEnum::OPEN_BRACE },
    { "}", PunctuatorEnum::CLOSE_BRACE },
    { ".", PunctuatorEnum::TERMINATOR },
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

PunctuatorEnum Punctuator::ForLexeme(std::string lexeme) {
    return dictionary.at(lexeme);
}

bool Punctuator::IsPunctuator(std::string buffer) {
    return dictionary.count(buffer);
}
