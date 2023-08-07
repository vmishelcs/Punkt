
#include <fstream>
#include <string>

#include "located_char.h"

/**
 * InputHandler class for reading the input file providing a stream of LocatedChars.
 */
class InputHandler {
public:
    InputHandler(std::string file_name);

    bool HasNext();

    LocatedChar Next();
    LocatedChar Peek();

    LocatedChar GetNextNonwhitespaceChar();

private:
    void PrepareNextLine();

    std::string input_file_name;
    std::ifstream input_file;
    std::string current_line;
    unsigned int current_line_number;
    unsigned int current_column_number;
};
