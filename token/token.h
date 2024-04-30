#ifndef TOKEN_H_
#define TOKEN_H_

#include <input_handler/text_location.h>

#include <memory>
#include <string>

enum class TokenType {
  PROGRAM_TOKEN,
  IDENTIFIER,
  KEYWORD,
  BOOLEAN_LITERAL,
  CHARACTER_LITERAL,
  INTEGER_LITERAL,
  STRING_LITERAL,
  PUNCTUATOR,
  EOF_TOKEN
};

class Token {
 public:
  Token(std::string lexeme, TextLocation location, TokenType token_type)
      : lexeme(lexeme), location(location), token_type(token_type) {}

  virtual std::unique_ptr<Token> CreateCopy() const = 0;

  bool IsEOF() const { return token_type == TokenType::EOF_TOKEN; }
  std::string GetLexeme() const { return lexeme; }
  TokenType GetTokenType() const { return token_type; }
  const TextLocation &GetLocation() const { return location; }
  virtual std::string ToString() const = 0;

  friend std::ostream &operator<<(std::ostream &os, const Token &t) {
    os << "(" << t.ToString() << ", " << t.location << ")";
    return os;
  }

 protected:
  std::string lexeme;
  TextLocation location;
  TokenType token_type;
};

#endif  // TOKEN_H_
