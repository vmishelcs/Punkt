#ifndef EOF_TOKEN_H_
#define EOF_TOKEN_H_

#include <input_handler/located_char.h>

#include <memory>
#include <string>

#include "token.h"

class EOFToken : public Token {
 public:
  EOFToken()
      : Token("EOF", LocatedChar::EOF_LOCATED_CHAR.location,
              TokenType::EOF_TOKEN) {}

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<EOFToken>();
  }

  virtual std::string ToString() const override { return "EOF TOKEN"; }
};

#endif  // EOF_TOKEN_H_
