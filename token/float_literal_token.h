#ifndef FLOAT_LITERAL_TOKEN_H_
#define FLOAT_LITERAL_TOKEN_H_

#include <memory>
#include <string>

#include "token.h"

class FloatLiteralToken : public Token {
 public:
  FloatLiteralToken(std::string lexeme, TextLocation location, double value)
      : Token(lexeme, location, TokenType::FLOAT_LITERAL), value(value) {}

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<FloatLiteralToken>(lexeme, location, value);
  }

  virtual std::string ToString() const override {
    std::string result =
        "FLOAT_LITERAL, value = " + std::to_string(this->value);
    return result;
  }

  double GetValue() const { return value; }

 private:
  double value;
};

#endif  // FLOAT_LITERAL_TOKEN_H_
