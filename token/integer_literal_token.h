#ifndef INTEGER_LITERAL_TOKEN_H_
#define INTEGER_LITERAL_TOKEN_H_

#include "token.h"

class IntegerLiteralToken : public Token {
public:
    IntegerLiteralToken(std::string lexeme, int value, TextLocation text_location);
    virtual std::string GetTokenString() const;
    int GetValue() const { return value; }
private:
    int value;
};

#endif // INTEGER_LITERAL_TOKEN_H_
