#ifndef PUNCTUATOR_H_
#define PUNCTUATOR_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "reserved_component.h"

enum class PunctuatorEnum {
    OPEN_BRACE,
    CLOSE_BRACE,
    TERMINATOR,
    EQUAL,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
};

class Punctuator : public ReservedComponent {
public:
    Punctuator(std::string lexeme);
    Punctuator(Punctuator&& punctuator);

    static bool IsPunctuator(std::string buffer);
    static int PunctuatorsWithPrefix(std::string prefix);
    static PunctuatorEnum ForLexeme(std::string buffer);

private:
    static std::vector<std::string> GetAllPrefixesForPunctuator(std::string punctuator_lexeme);

    PunctuatorEnum punctuator_enum;
    static std::unordered_map<std::string, PunctuatorEnum> dictionary;
    static std::unordered_map<std::string, int> num_punctuators_with_prefix;
};

#endif // PUNCTUATOR_H_
