#include "located_char_stream.h"

LocatedCharStream::LocatedCharStream(InputHandler *input_handler)
    : next_char(FLAG_END_OF_INPUT) {
    this->input_handler = input_handler;
    this->next_char = input_handler->Next();
}

LocatedCharStream::~LocatedCharStream() {
    delete this->input_handler;
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
