#ifndef INTEGER_LITERAL_TOKEN_H_
#define INTEGER_LITERAL_TOKEN_H_

#include <cstdint>
#include <memory>
#include <string>

#include "token.h"

class IntegerLiteralToken : public Token {
 public:
  IntegerLiteralToken(std::string lexeme, TextLocation location, int64_t value)
      : Token(lexeme, location, TokenType::INTEGER_LITERAL), value(value) {}

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<IntegerLiteralToken>(lexeme, location, value);
  }

  virtual std::string ToString() const override {
    std::string result =
        "INTEGER_LITERAL, value = " + std::to_string(this->value);
    return result;
  }

  int64_t GetValue() const { return value; }

 private:
  int64_t value;
};

#endif  // INTEGER_LITERAL_TOKEN_H_
