#ifndef IDENTIFIER_TOKEN_H_
#define IDENTIFIER_TOKEN_H_

#include "token.h"

class IdentifierToken : public Token {
public:
    IdentifierToken(std::string lexeme, TextLocation location)
        : Token(lexeme, location, TokenType::IDENTIFIER)
    {}
    
    virtual std::string AsString() const override {
        std::string result = "IDENTIFIER, " + this->GetLexeme();
        return result;
    }
};

#endif // IDENTIFIER_TOKEN_H_
