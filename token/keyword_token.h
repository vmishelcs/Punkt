#ifndef KEYWORD_TOKEN_H_
#define KEYWORD_TOKEN_H_

#include <scanner/keyword.h>

#include <memory>

#include "token.h"

class KeywordToken : public Token {
 public:
  KeywordToken(std::string lexeme, TextLocation location, Keyword keyword)
      : Token(lexeme, location, TokenType::KEYWORD), keyword(keyword) {}

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<KeywordToken>(lexeme, location, keyword);
  }

  virtual std::string ToString() const override {
    std::string result = "KEYWORD, " + this->GetLexeme();
    return result;
  }

  Keyword GetKeywordEnum() const { return keyword; }

  static bool IsTokenKeyword(Token *token,
                             std::initializer_list<Keyword> keywords) {
    KeywordToken *keyword_token = dynamic_cast<KeywordToken *>(token);
    if (!keyword_token) {
      return false;
    }

    for (const auto &keyword : keywords) {
      if (keyword_token->GetKeywordEnum() == keyword) {
        return true;
      }
    }
    return false;
  }

 private:
  Keyword keyword;
};

#endif  // KEYWORD_TOKEN_H_
