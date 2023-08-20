#ifndef PUNCTUATOR_H_
#define PUNCTUATOR_H_

#include <string>
#include <unordered_map>
#include <string>

#include "reserved_component.h"

enum class PunctuatorEnum {
    OPEN_BRACE,
    CLOSE_BRACE,
    TERMINATOR,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
};

class Punctuator : public ReservedComponent {
public:
    Punctuator(std::string lexeme);
    Punctuator(Punctuator&& keyword);

    static bool IsPunctuator(std::string buffer);
    static PunctuatorEnum ForLexeme(std::string buffer);

private:
    PunctuatorEnum punctuator_enum;
    static std::unordered_map<std::string, PunctuatorEnum> dictionary;
};

#endif // PUNCTUATOR_H_
