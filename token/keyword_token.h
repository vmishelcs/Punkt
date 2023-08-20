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

    virtual std::string GetTokenString() const {
        std::string result = "KEYWORD, " + this->GetLexeme();
        return result;
    }

private:
    Keyword keyword;
};

#endif // KEYWORD_TOKEN_H_
