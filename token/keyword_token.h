#ifndef KEYWORD_TOKEN_H_
#define KEYWORD_TOKEN_H_

#include <scanner/keyword.h>

#include "token.h"

class KeywordToken : public Token {
public:
    KeywordToken(std::string lexeme, TextLocation location, Keyword keyword)
        : Token(lexeme, location, TokenType::KEYWORD)
        , keyword(std::move(keyword))
    {}

    virtual std::string AsString() const override {
        std::string result = "KEYWORD, " + this->GetLexeme();
        return result;
    }

    KeywordEnum GetKeywordEnum() const {
        return keyword.GetKeywordEnum();
    }

    static bool IsTokenKeyword(Token& token, std::initializer_list<KeywordEnum> keywords) {
        if (token.GetTokenType() != TokenType::KEYWORD) {
            return false;
        }

        KeywordToken& keyword_token = dynamic_cast<KeywordToken&>(token);
        for (const auto& keyword : keywords) {
            if (keyword_token.GetKeywordEnum() == keyword) {
                return true;
            }
        }
        return false;
    }

private:
    Keyword keyword;
};

#endif // KEYWORD_TOKEN_H_
