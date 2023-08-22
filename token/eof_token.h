#ifndef EOF_TOKEN_H_
#define EOF_TOKEN_H_

#include <input_handler/located_char.h>

#include "token.h"

class EOFToken : public Token {
public:
    EOFToken()
        : Token("", LocatedChar::EOF_LOCATED_CHAR.location, TokenType::EOF_TOKEN)
    {}

    virtual std::string GetTokenString() const {
        return "EOF TOKEN";
    }
};

#endif // EOF_TOKEN_H_
