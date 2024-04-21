#ifndef PUNCTUATOR_TOKEN_H_
#define PUNCTUATOR_TOKEN_H_

#include <scanner/punctuator.h>

#include "token.h"

class PunctuatorToken : public Token {
 public:
  PunctuatorToken(std::string lexeme, TextLocation location,
                  Punctuator punctuator)
      : Token(lexeme, location, TokenType::PUNCTUATOR),
        punctuator(std::move(punctuator)) {}

  virtual std::string ToString() const override {
    std::string result = "PUNCTUATOR, \'" + this->GetLexeme() + "\'";
    return result;
  }

  PunctuatorEnum GetPunctuatorEnum() const {
    return punctuator.GetPunctuatorEnum();
  }

  static bool IsTokenPunctuator(
      Token *token, std::initializer_list<PunctuatorEnum> punctuators) {
    PunctuatorToken *punctuator_token = dynamic_cast<PunctuatorToken *>(token);
    if (!punctuator_token) {
      return false;
    }

    for (const auto &punctuator : punctuators) {
      if (punctuator_token->GetPunctuatorEnum() == punctuator) {
        return true;
      }
    }
    return false;
  }

 private:
  Punctuator punctuator;
};

#endif  // PUNCTUATOR_TOKEN_H_
