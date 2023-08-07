#include <input_handler/input_handler.h>

InputHandler::InputHandler(std::string file_name) {
    if (file_name.length() == 0) {
        throw std::runtime_error("Input file name length must be greater than 0");
    }
    this->input_file_name = file_name;
    this->input_file = std::ifstream(this->input_file_name, std::ios::in);
    if (!this->input_file.is_open()) {
        throw std::runtime_error("Failed to open input file " + file_name);          
    }
    
    this->current_line_number = 0;
    PrepareNextLine();
}

bool InputHandler::HasNext() {
    if (this->current_column_number > this->current_line.length()
        && this->input_file.eof()) {
        return false;
    }
    return true;
}

LocatedChar InputHandler::Next() {
    if (current_column_number > this->current_line.length()) {
        PrepareNextLine();
    }

    char c = this->current_line[this->current_column_number - 1];
    return LocatedChar(c, this->current_line_number, this->current_column_number++);
}

//LocatedChar InputHandler::Peek() {
//
//}

void InputHandler::PrepareNextLine() {
    std::getline(this->input_file, this->current_line);
    if (this->input_file.fail()
        && this->current_line.length() == 0
        && !this->input_file.eof()) {
        throw std::runtime_error("Failed to read line "
            + std::to_string(this->current_line_number)
            + " from " + this->input_file_name);
    }
    current_column_number = 1;
    ++current_line_number;
}


