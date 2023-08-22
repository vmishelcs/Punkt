#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include <deque>
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
    void PutBack(LocatedChar ch);

private:
    bool LineHasMoreChars() const;
    bool FileHasMoreLines() const;
    void PreloadNextLine();

    std::string input_file_name;
    std::ifstream input_file;
    std::deque<LocatedChar> char_stream;
    unsigned int line_num;
    unsigned int column_num;
};

#endif // INPUT_HANDLER_H_
