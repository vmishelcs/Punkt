#ifndef BOOLEAN_LITERAL_TOKEN_H_
#define BOOLEAN_LITERAL_TOKEN_H_

#include "token.h"

class BooleanLiteralToken : public Token {
 public:
  BooleanLiteralToken(std::string lexeme, TextLocation text_location,
                      bool value)
      : Token(lexeme, text_location, TokenType::BOOLEAN_LITERAL),
        value(value) {}

  virtual std::string ToString() const override {
    std::string result =
        "BOOLEAN_LITERAL, value = " + std::to_string(this->value);
    return result;
  }

  bool GetValue() const { return value; }

 private:
  bool value;
};

#endif  // BOOLEAN_LITERAL_TOKEN_H_
