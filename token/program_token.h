#ifndef PROGRAM_TOKEN_H_
#define PROGRAM_TOKEN_H_

#include <memory>
#include <string>

#include "token.h"

class ProgramToken : public Token {
 public:
  ProgramToken()
      : Token("Punkt", TextLocation("", 0, 0), TokenType::PROGRAM_TOKEN) {}

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<ProgramToken>();
  }

  virtual std::string ToString() const override {
    std::string result = "PROGRAM, " + this->GetLexeme();
    return result;
  }
};

#endif  // PROGRAM_TOKEN_H_
