#ifndef TOKEN_H_
#define TOKEN_H_

#include <input_handler/text_location.h>

#include <memory>
#include <string>

enum class TokenType {
  BOOLEAN_LITERAL,
  CHARACTER_LITERAL,
  EOF_TOKEN,
  IDENTIFIER,
  INTEGER_LITERAL,
  KEYWORD,
  OPERATOR_TOKEN,
  PLACEHOLDER_TOKEN,
  PROGRAM_TOKEN,
  PUNCTUATOR,
  STRING_LITERAL,
};

class Token {
 public:
  Token(std::string lexeme, TextLocation location, TokenType token_type)
      : lexeme(lexeme), location(location), token_type(token_type) {}

  Token(const Token &token, TokenType token_type)
      : lexeme(token.lexeme),
        location(token.location),
        token_type(token_type) {}

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
