#ifndef KEYWORD_H_
#define KEYWORD_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <string>

enum class KeywordEnum {
    MAIN,
    VAR
};

class Keyword {
public:
    Keyword(std::string lexeme);
    Keyword(Keyword&& keyword);
    std::string GetLexeme() const;

    static bool IsKeyword(std::string buffer);
    static KeywordEnum ForLexeme(std::string buffer);

private:
    std::string lexeme;
    KeywordEnum enum_value;
    static std::unordered_map<std::string, KeywordEnum> dictionary;
};

#endif // KEYWORD_H_
