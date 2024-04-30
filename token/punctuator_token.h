#ifndef PUNCTUATOR_TOKEN_H_
#define PUNCTUATOR_TOKEN_H_

#include <scanner/punctuator.h>

#include "token.h"

class PunctuatorToken : public Token {
 public:
  PunctuatorToken(std::string lexeme, TextLocation location,
                  Punctuator punctuator_enum)
      : Token(lexeme, location, TokenType::PUNCTUATOR),
        punctuator_enum(punctuator_enum) {}

  virtual std::string ToString() const override {
    std::string result = "PUNCTUATOR, \'" + this->GetLexeme() + "\'";
    return result;
  }

  Punctuator GetPunctuatorEnum() const { return punctuator_enum; }

  static bool IsTokenPunctuator(Token *token,
                                std::initializer_list<Punctuator> punctuators) {
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
  Punctuator punctuator_enum;
};

#endif  // PUNCTUATOR_TOKEN_H_
