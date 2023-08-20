#include <stdexcept>

#include "keyword.h"

std::unordered_map<std::string, KeywordEnum> Keyword::dictionary = {
    { "main", KeywordEnum::MAIN },
    { "var",  KeywordEnum::VAR  }
};

Keyword::Keyword(std::string lexeme) {
    this->lexeme = lexeme;
    this->enum_value = dictionary[lexeme];
}

Keyword::Keyword(Keyword&& keyword) {
    this->lexeme = std::move(keyword.lexeme);
    this->enum_value = std::move(keyword.enum_value);
}

std::string Keyword::GetLexeme() const {
    return lexeme;
}

KeywordEnum Keyword::ForLexeme(std::string lexeme) {
    return dictionary.at(lexeme);
}

bool Keyword::IsKeyword(std::string buffer) {
    return dictionary.count(buffer);
}
