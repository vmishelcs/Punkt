#ifndef PROGRAM_TOKEN_H_
#define PROGRAM_TOKEN_H_

#include "token.h"

class ProgramToken : public Token {
public:
    ProgramToken()
        : Token("Punkt", TextLocation("", 0, 0), TokenType::PROGRAM_TOKEN)
    {}

    virtual std::string GetTokenString() const {
        std::string result = "PROGRAM_TOKEN, " + this->GetLexeme();
        return result;
    }
};

#endif // PROGRAM_TOKEN_H_
