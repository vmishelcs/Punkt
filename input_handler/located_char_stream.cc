#include "located_char_stream.h"

LocatedCharStream::LocatedCharStream(std::string input_file_name)
    : next_char(FLAG_END_OF_INPUT) {
    this->input_handler = std::make_unique<InputHandler>(input_file_name);
    this->next_char = input_handler->Next();
}

LocatedChar LocatedCharStream::Next() {
    LocatedChar result = this->next_char;
    this->next_char = this->input_handler->Next();
    return result;
}

LocatedChar LocatedCharStream::NextNonwhitespaceChar() {
    while (this->next_char.IsWhitespace()) {
        this->next_char = this->input_handler->Next();
    }
    LocatedChar result = this->next_char;
    this->next_char = this->input_handler->Next();
    return result;
}

LocatedChar LocatedCharStream::Peek() {
    return this->next_char;
}
