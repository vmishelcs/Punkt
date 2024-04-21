#ifndef CHARACTER_LITERAL_TOKEN_H_
#define CHARACTER_LITERAL_TOKEN_H_

#include "token.h"

class CharacterLiteralToken : public Token {
 public:
  CharacterLiteralToken(std::string lexeme, TextLocation text_location,
                        char value)
      : Token(lexeme, text_location, TokenType::CHARACTER_LITERAL),
        value(value) {}

  virtual std::string ToString() const override {
    std::string result =
        "CHARACTER_LITERAL, value = " + std::to_string(this->value);
    return result;
  }

  char GetValue() const { return value; }

 private:
  char value;
};

#endif  // CHARACTER_LITERAL_TOKEN_H_
