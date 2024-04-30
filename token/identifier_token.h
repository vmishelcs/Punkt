#ifndef IDENTIFIER_TOKEN_H_
#define IDENTIFIER_TOKEN_H_

#include <memory>
#include <string>

#include "token.h"

class IdentifierToken : public Token {
 public:
  IdentifierToken(std::string lexeme, TextLocation location)
      : Token(lexeme, location, TokenType::IDENTIFIER) {}

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<IdentifierToken>(lexeme, location);
  }

  virtual std::string ToString() const override {
    std::string result = "IDENTIFIER, " + this->GetLexeme();
    return result;
  }
};

#endif  // IDENTIFIER_TOKEN_H_
