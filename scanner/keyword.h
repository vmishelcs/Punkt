#ifndef KEYWORD_H_
#define KEYWORD_H_

#include <string>
#include <unordered_map>
#include <string>

#include "reserved_component.h"

enum class KeywordEnum {
    MAIN,
    VAR
};

class Keyword : public ReservedComponent {
public:
    Keyword(std::string lexeme);
    Keyword(Keyword&& keyword);

    KeywordEnum GetKeywordEnum() const;

    static bool IsKeyword(std::string buffer);
    static KeywordEnum ForLexeme(std::string buffer);

private:
    KeywordEnum keyword_enum;
    static std::unordered_map<std::string, KeywordEnum> dictionary;
};

#endif // KEYWORD_H_
