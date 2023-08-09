
#include "identifier_token.h"

IdentifierToken::IdentifierToken(std::string lexeme, TextLocation location)
    : Token(lexeme, location) {
}

std::string IdentifierToken::GetTokenString() const {
    std::string result = "IDENTIFIER, " + this->GetLexeme();
    return result;
}
