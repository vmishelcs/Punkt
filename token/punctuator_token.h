#ifndef PUNCTUATOR_TOKEN_H_
#define PUNCTUATOR_TOKEN_H_

#include <scanner/punctuator.h>

#include "token.h"

class PunctuatorToken : public Token {
public:
    PunctuatorToken(std::string lexeme, TextLocation location, Punctuator punctuator)
        : Token(lexeme, location, TokenType::PUNCTUATOR)
        , punctuator(std::move(punctuator))
    {}

    virtual std::string GetTokenString() const {
        std::string result = "PUNCTUATOR, " + this->GetLexeme();
        return result;
    }
    
private:
    Punctuator punctuator;
};

#endif // PUNCTUATOR_TOKEN_H_
