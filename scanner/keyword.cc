#include "keyword.h"

std::unordered_map<std::string, KeywordEnum> Keyword::dictionary = {
    { "const", KeywordEnum::CONST },
    { "else",  KeywordEnum::ELSE  },
    { "false", KeywordEnum::FALSE },
    { "if",    KeywordEnum::IF    },
    { "main",  KeywordEnum::MAIN  },
    { "print", KeywordEnum::PRINT },
    { "true",  KeywordEnum::TRUE  },
};

std::unordered_map<KeywordEnum, std::string> Keyword::reverse_dictionary = [] {
    std::unordered_map<KeywordEnum, std::string> result;
    for (const auto& [key, value] : dictionary) {
        result[value] = key;
    }
    return result;
}();

Keyword::Keyword(std::string lexeme) : ReservedComponent(lexeme) {
    this->keyword_enum = dictionary.at(lexeme);
}

Keyword::Keyword(Keyword&& keyword) : ReservedComponent(std::move(keyword)) {
    this->keyword_enum = std::move(keyword.keyword_enum);
}

KeywordEnum Keyword::GetKeywordEnum() const {
    return keyword_enum;
}

KeywordEnum Keyword::ForLexeme(std::string lexeme) {
    return dictionary.at(lexeme);
}

std::string Keyword::ForKeywordEnum(KeywordEnum keyword_enum) {
    return reverse_dictionary.at(keyword_enum);
}

bool Keyword::IsKeyword(std::string buffer) {
    return dictionary.contains(buffer);
}
