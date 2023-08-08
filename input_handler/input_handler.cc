
#include "input_handler.h"

InputHandler::InputHandler(std::string input_file_name) {
    this->input_file_name = input_file_name;
    this->input_file = std::ifstream(this->input_file_name, std::ios::in);
    if (!this->input_file.is_open()) {
        throw std::runtime_error("Failed to open input file " + this->input_file_name);
    }

    this->current_line = "";
    this->line_num = 0;
    this->column_num = 1;

    while (!LineHasMoreChars() && FileHasMoreLines()) {
        PreloadNextLine();
    }
}

LocatedChar InputHandler::Next() {
    while (!LineHasMoreChars() && FileHasMoreLines()) {
        PreloadNextLine();
    }
    if (LineHasMoreChars()) {
        char c = this->current_line[this->column_num - 1];
        return LocatedChar(
            c,
            this->input_file_name,
            this->line_num,
            this->column_num++
        );
    }
    return FLAG_END_OF_INPUT;
}

bool InputHandler::LineHasMoreChars() {
    return this->column_num <= this->current_line.length();
}

bool InputHandler::FileHasMoreLines() {
    return !this->input_file.eof();
}

void InputHandler::PreloadNextLine() {
    std::getline(this->input_file, this->current_line);
    if (this->input_file.fail()
        && this->current_line.length() == 0
        && !this->input_file.eof()) {
        throw std::runtime_error("Failed to read line "
            + std::to_string(this->line_num)
            + " from " + this->input_file_name);
    }
    this->column_num = 1;
    this->line_num += 1;
}


