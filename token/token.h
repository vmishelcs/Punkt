#ifndef TOKEN_H_
#define TOKEN_H_

#include <input_handler/text_location.h>

class Token {
public:
    Token(std::string lexeme, TextLocation location)
        : lexeme(lexeme)
        , location(location)
    {}

    std::string GetLexeme() const { return lexeme; };
    virtual std::string GetTokenString() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Token& t) {
        os << "(" << t.GetTokenString() << ", " << t.location << ")";
        return os;
    }

private:
    std::string lexeme;
    TextLocation location;
};

#endif // TOKEN_H_
