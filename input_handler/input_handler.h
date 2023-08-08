#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_


#include <fstream>
#include <string>

#include "located_char.h"

/**
 * InputHandler class for reading the input file providing a stream of LocatedChars.
 */
class InputHandler {
public:
    InputHandler(std::string input_file_name);
    ~InputHandler();

    LocatedChar Next();

private:
    bool LineHasMoreChars();
    bool FileHasMoreLines();
    void PreloadNextLine();

    std::string input_file_name;
    std::ifstream input_file;
    std::string current_line;
    unsigned int line_num;
    unsigned int column_num;
};

static const LocatedChar FLAG_END_OF_INPUT(0, "null", 0, 0);

#endif // INPUT_HANDLER_H_
