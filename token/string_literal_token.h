#ifndef STRING_LITERAL_TOKEN_H_
#define STRING_LITERAL_TOKEN_H_

#include <memory>
#include <string>

#include "token.h"

class StringLiteralToken : public Token {
 public:
  StringLiteralToken(std::string lexeme, TextLocation text_location,
                     std::string value)
      : Token(lexeme, text_location, TokenType::STRING_LITERAL), value(value) {}

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<StringLiteralToken>(lexeme, location, value);
  }

  virtual std::string ToString() const override {
    std::string result = "STRING_LITERAL, value = \"" + value + "\"";
    return result;
  }

  std::string GetValue() const { return value; }

 private:
  std::string value;
};

#endif  // STRING_LITERAL_TOKEN_H_
