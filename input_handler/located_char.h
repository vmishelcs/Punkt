#ifndef LOCATED_CHAR_H_
#define LOCATED_CHAR_H_

#include <iostream>

#include "text_location.h"

/**
 * LocatedChar struct used for keeping track of individual character locations within
 * the input file.
 */
struct LocatedChar {
    LocatedChar(char c, std::string file_name, unsigned int line, unsigned int column)
        : character(c) 
        , location(file_name, line, column)
    {}

    char character;
    TextLocation location;
};

inline std::ostream& operator<<(std::ostream& os, const LocatedChar& lc) {
    os << lc.character
        << " (" << lc.location.file_name
        << ":" << lc.location.line
        << ":" << lc.location.column << ")";
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
