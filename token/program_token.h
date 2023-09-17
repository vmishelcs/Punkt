#ifndef PROGRAM_TOKEN_H_
#define PROGRAM_TOKEN_H_

#include "token.h"

class ProgramToken : public Token {
public:
    ProgramToken()
        : Token("Punkt", TextLocation("", 0, 0), TokenType::PROGRAM_TOKEN)
    {}

    virtual std::string AsString() const override {
        std::string result = "PROGRAM, " + this->GetLexeme();
        return result;
    }
};

#endif // PROGRAM_TOKEN_H_
