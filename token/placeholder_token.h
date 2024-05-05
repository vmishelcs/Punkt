#ifndef PLACEHOLDER_TOKEN_H_
#define PLACEHOLDER_TOKEN_H_

#include "token.h"

/// @brief Placeholder token that does not represent any particular part of the
/// Punkt language. This object is to be used to construct nodes that have no
/// correspondence to any specific lexical construct of Punkt (e.g.
/// LambdaParameterNode, NopNode).
class PlaceholderToken : public Token {
 public:
  PlaceholderToken(const Token& token)
      : Token(token, TokenType::PLACEHOLDER_TOKEN) {}

  static std::unique_ptr<PlaceholderToken> Create(const Token& token) {
    return std::make_unique<PlaceholderToken>(token);
  }

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<PlaceholderToken>(*this);
  }

  virtual std::string ToString() const { return "placeholder token"; }
};

#endif  // PLACEHOLDER_TOKEN_H_
