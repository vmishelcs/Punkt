#include <iostream>

/**
 * LocatedChar struct used for keeping track of individual character locations within
 * the input file.
 */
struct LocatedChar {
    LocatedChar(char c, unsigned int line, unsigned int column)
        : character(c) 
        , line_number(line)
        , column_number(column)
    {}

    friend std::ostream& operator<<(std::ostream& os, const LocatedChar& lc) {
        os << "(char: " << lc.character
            << ", line: " << lc.line_number
            << ", column: " << lc.column_number << ")";
        return os;
    }

    char character;
    unsigned int line_number;
    unsigned int column_number;
};
