#ifndef PUNCTUATOR_TOKEN_H_
#define PUNCTUATOR_TOKEN_H_

#include <scanner/punctuator.h>

#include "token.h"

class PunctuatorToken : public Token {
 public:
  PunctuatorToken(std::string lexeme, TextLocation location,
                  Punctuator punctuator)
      : Token(lexeme, location, TokenType::PUNCTUATOR),
        punctuator(punctuator) {}

  virtual std::string ToString() const override {
    std::string result = "PUNCTUATOR, \'" + this->GetLexeme() + "\'";
    return result;
  }

  Punctuator GetPunctuator() const { return punctuator; }

  static bool IsTokenPunctuator(Token *token,
                                std::initializer_list<Punctuator> punctuators) {
    PunctuatorToken *punctuator_token = dynamic_cast<PunctuatorToken *>(token);
    if (!punctuator_token) {
      return false;
    }

    for (const auto &punctuator : punctuators) {
      if (punctuator_token->GetPunctuator() == punctuator) {
        return true;
      }
    }
    return false;
  }

 private:
  Punctuator punctuator;
};

#endif  // PUNCTUATOR_TOKEN_H_
