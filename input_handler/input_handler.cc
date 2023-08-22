#include "input_handler.h"

const LocatedChar LocatedChar::EOF_LOCATED_CHAR(0, "null", -1, -1);

InputHandler::InputHandler(std::string input_file_name) : input_file_name(input_file_name) {
    if (input_file_name.length() == 0) {
        throw new std::runtime_error("File name length must be greater than 0");
    }
    input_file = std::ifstream(input_file_name, std::ios::in);
    if (!input_file.is_open()) {
        std::string message = "Failed to open input file ";
        message.append(input_file_name);
        throw new std::runtime_error(message);
    }

    line_num = 0;
    column_num = 1;
    while (char_stream.empty() && FileHasMoreLines()) {
        PreloadNextLine();
    }
}

InputHandler::~InputHandler() {
    input_file.close();
}

LocatedChar InputHandler::Next() {
    while (!LineHasMoreChars() && FileHasMoreLines()) {
        PreloadNextLine();
    }
    if (LineHasMoreChars()) {
        LocatedChar lc = char_stream.front();
        char_stream.pop_front();
        return lc;
    }
    return LocatedChar::EOF_LOCATED_CHAR;
}

void InputHandler::PutBack(LocatedChar ch) {
    char_stream.push_front(ch);
}

bool InputHandler::LineHasMoreChars() const {
    return !char_stream.empty();
}

bool InputHandler::FileHasMoreLines() const {
    return !input_file.eof();
}

void InputHandler::PreloadNextLine() {
    char c = 0;
    column_num = 1;
    line_num += 1;
    while (input_file >> std::noskipws >> c && c != '\n') {
        char_stream.push_back(LocatedChar(c, input_file_name, line_num, column_num));
        ++column_num;
    }
    if (!input_file.eof()) {
        char_stream.push_back(LocatedChar('\n', input_file_name, line_num, column_num));
        ++column_num;
    }
}
