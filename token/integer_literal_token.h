#ifndef INTEGER_LITERAL_TOKEN_H_
#define INTEGER_LITERAL_TOKEN_H_

#include "token.h"

class IntegerLiteralToken : public Token {
 public:
  IntegerLiteralToken(std::string lexeme, TextLocation location, int value)
      : Token(lexeme, location, TokenType::INTEGER_LITERAL), value(value) {}

  virtual std::string ToString() const override {
    std::string result =
        "INTEGER_LITERAL, value = " + std::to_string(this->value);
    return result;
  }

  int GetValue() const { return value; }

 private:
  int value;
};

#endif  // INTEGER_LITERAL_TOKEN_H_
