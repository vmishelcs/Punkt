
/**
 * LocatedChar class used for keeping track of individual character location within the input file.
 */
class LocatedChar {
  public:
    LocatedChar(char c, unsigned int line, unsigned int column)
        : character(c) 
        , line_number(line)
        , column_number(column)
    {}

    inline char GetCharacter() { return this->character; }
    inline unsigned int GetLineNumber() { return this->line_number; }
    inline unsigned int GetColumnNumber() {return this->column_number; }

  private:
    char character;
    unsigned int line_number;
    unsigned int column_number;
};
