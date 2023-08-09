#ifndef IDENTIFIER_TOKEN_H_
#define IDENTIFIER_TOKEN_H_

#include <string>

#include "token.h"

class IdentifierToken : public Token {
public:
    IdentifierToken(std::string lexeme, TextLocation location);
    virtual std::string GetTokenString() const;
};

#endif // IDENTIFIER_TOKEN_H_
