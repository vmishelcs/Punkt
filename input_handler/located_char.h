
/**
 * LocatedChar class used for keeping track of individual character location within the input file.
 */
class LocatedChar {
  public:
    inline LocatedChar(char c, int line, int column) :
        character(c), line_number(line_number), column_number(column_number) {}
    inline char GetCharacter() { return this->character; }
    inline unsigned int GetLineNumber() { return this->line_number; }
    inline unsigned int GetColumnNumber() {return this->column_number; }

  private:
    char character;
    unsigned int line_number;
    unsigned int column_number;
};
