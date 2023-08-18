#include "integer_literal_token.h"

IntegerLiteralToken::IntegerLiteralToken(
    std::string lexeme,
    int value,
    TextLocation text_location)
    : Token(lexeme, text_location, TokenType::INTEGER_LITERAL)
    , value(value)
{}

std::string IntegerLiteralToken::GetTokenString() const {
    std::string result = "INTEGER_LITERAL, "
    + this->GetLexeme()
    + ", value = " + std::to_string(this->value);
    return result;
}
