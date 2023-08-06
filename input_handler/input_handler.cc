
#include <input_handler/input_handler.h>

InputHandler::InputHandler(std::string file_name)
{
    this->input_file = std::ifstream(file_name, std::ios::in);
    if (!this->input_file.is_open()) {
        throw std::runtime_error("Failed to open input file " + file_name);          
    }
    this->current_line_number = 1;
    this->current_column_number = 1;
}
