
#include "input_handler.h"

InputHandler::InputHandler(std::string file_name)
{
    this->input_file = std::ifstream(file_name, std::ios::in);
    if (!this->input_file.is_open()) {

    }
}
