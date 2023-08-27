#ifndef TOKEN_H_
#define TOKEN_H_

#include <input_handler/text_location.h>

enum class TokenType {
    PROGRAM_TOKEN,
    IDENTIFIER,
    KEYWORD,
    INTEGER_LITERAL,
    PUNCTUATOR,
    EOF_TOKEN
};

class Token {
public:
    Token(std::string lexeme, TextLocation location, TokenType token_type)
        : lexeme(lexeme)
        , location(location)
        , token_type(token_type)
    {}

    std::string GetLexeme() const { return lexeme; }
    TokenType GetTokenType() const { return token_type; }
    virtual std::string GetTokenString() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Token& t) {
        os << "(" << t.GetTokenString() << ", " << t.location << ")";
        return os;
    }

private:
    std::string lexeme;
    TextLocation location;
    TokenType token_type;
};

#endif // TOKEN_H_
