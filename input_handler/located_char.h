#ifndef LOCATED_CHAR_H_
#define LOCATED_CHAR_H_

#include <scanner/punctuator.h>

#include <cctype>
#include <iostream>

#include "text_location.h"

/**
 * LocatedChar struct used for keeping track of individual character locations
 * within the input file.
 */
struct LocatedChar {
  LocatedChar(char c, std::string file_name, unsigned int line,
              unsigned int column)
      : character(c), location(file_name, line, column) {}

  LocatedChar(const LocatedChar& lc)
      : character(lc.character), location(lc.location) {}

  bool IsWhitespace() const { return isspace(character); }
  bool IsCommentStart() const { return character == '#'; }
  bool IsIdentifierStart() const {
    return isalpha(character) || (character == '_');
  }
  bool IsIdentifierChar() const {
    return isalnum(character) || (character == '_');
  }
  bool IsNumberStart() const { return isdigit(character); }
  bool IsDigit() const { return isdigit(character); }
  bool IsPunctuatorStart() const {
    return Punctuator::PunctuatorsWithPrefix(std::string(1, character)) > 0;
  }
  bool IsStringStart() const { return character == '\"'; }
  bool IsCharacterStart() const { return character == '\''; }
  bool IsPartOfComment() const { return character != '#' && character != '\n'; }

  char character;
  TextLocation location;
  static LocatedChar EOF_LOCATED_CHAR;
  static LocatedChar NULL_CHAR;
};

inline std::ostream& operator<<(std::ostream& os, const LocatedChar& lc) {
  os << lc.character << " (" << lc.location << ")";
  return os;
}

inline bool operator==(const LocatedChar& left, const LocatedChar& right) {
  return left.character == right.character &&
         left.location.file_name == right.location.file_name &&
         left.location.line == right.location.line &&
         left.location.column == right.location.column;
}

inline bool operator!=(const LocatedChar& left, const LocatedChar& right) {
  return !(left == right);
}

#endif  // LOCATED_CHAR_H_
