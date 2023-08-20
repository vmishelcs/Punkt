#include "keyword.h"

std::unordered_map<std::string, KeywordEnum> Keyword::dictionary = {
    { "main", KeywordEnum::MAIN },
    { "var",  KeywordEnum::VAR  }
};

Keyword::Keyword(std::string lexeme) : ReservedComponent(lexeme) {
    this->keyword_enum = dictionary.at(lexeme);
}

Keyword::Keyword(Keyword&& keyword) : ReservedComponent(std::move(keyword)) {
    this->keyword_enum = std::move(keyword.keyword_enum);
}

KeywordEnum Keyword::ForLexeme(std::string lexeme) {
    return dictionary.at(lexeme);
}

bool Keyword::IsKeyword(std::string buffer) {
    return dictionary.count(buffer);
}
