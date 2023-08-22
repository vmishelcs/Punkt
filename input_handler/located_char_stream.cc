#include "located_char_stream.h"

LocatedCharStream::LocatedCharStream(std::string input_file_name)
    : next_char(LocatedChar::EOF_LOCATED_CHAR) {
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

LocatedChar LocatedCharStream::Peek() const {
    return this->next_char;
}

void LocatedCharStream::PutBack(LocatedChar ch) {
    input_handler->PutBack(ch);
}

bool LocatedCharStream::HasNext() const {
    return this->next_char != LocatedChar::EOF_LOCATED_CHAR;
}
