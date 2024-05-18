#ifndef OPERATOR_TOKEN_H_
#define OPERATOR_TOKEN_H_

#include <scanner/operator.h>

#include <memory>

#include "token.h"

class OperatorToken : public Token {
 public:
  OperatorToken(std::string lexeme, TextLocation location, Operator op)
      : Token(lexeme, location, TokenType::OPERATOR_TOKEN), op(op) {}

  virtual std::unique_ptr<Token> CreateCopy() const override {
    return std::make_unique<OperatorToken>(lexeme, location, op);
  }

  virtual std::string ToString() const override {
    return "OPERATOR \'" + lexeme + "\'";
  }

  Operator GetOperatorEnum() const { return op; }

  /// @brief Check if the specified token represents any of the provided
  /// operators.
  /// @param token Token to check if it describes any of the provided operators.
  /// @param ops List of `Operator` enums.
  /// @return `true` if this token represents any of the provided operators,
  /// `false` otherwise.
  static bool IsTokenOperator(Token *token,
                              std::initializer_list<Operator> ops) {
    OperatorToken *operator_token = dynamic_cast<OperatorToken *>(token);
    if (!operator_token) {
      return false;
    }

    for (Operator op : ops) {
      if (op == operator_token->GetOperatorEnum()) {
        return true;
      }
    }
    return false;
  }

 private:
  Operator op;
};

#endif  // OPERATOR_TOKEN_H_
