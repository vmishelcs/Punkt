#ifndef LOCATED_CHAR_H_
#define LOCATED_CHAR_H_

#include <cctype>
#include <iostream>
#include <locale>

#include "text_location.h"

/**
 * LocatedChar struct used for keeping track of individual character locations within the input 
 * file.
 */
struct LocatedChar {
    LocatedChar(char c, std::string file_name, unsigned int line, unsigned int column)
        : character(c) 
        , location(file_name, line, column)
    {}

    LocatedChar(const LocatedChar& lc)
        : character(lc.character)
        , location(lc.location)
    {}

    bool IsWhitespace() const { return std::isspace(character); }
    bool IsEndOfInput() const {
        return this->character == 0
            && this->location.file_name == "null"
            && this->location.line == 0
            && this->location.column == 0;
    }
    bool IsIdentifierStart() const {
        return ('A' <= character && character <= 'Z')
            || ('a' <= character && character <= 'z')
            || (character == '_');
    }
    bool IsIdentifierChar() const {
        return ('A' <= character && character <= 'Z')
            || ('a' <= character && character <= 'z')
            || ('0' <= character && character <= '9')
            || (character == '_');
    }

    char character;
    TextLocation location;
};

inline std::ostream& operator<<(std::ostream& os, const LocatedChar& lc) {
    os << lc.character << " (" << lc.location << ")";
    return os;
}

inline bool operator==(const LocatedChar& left, const LocatedChar& right) {
    return left.character == right.character
        && left.location.file_name == right.location.file_name
        && left.location.line == right.location.line
        && left.location.column == right.location.column;
}

inline bool operator!=(const LocatedChar& left, const LocatedChar& right) {
    return !(left == right);
}

#endif // LOCATED_CHAR_H_
