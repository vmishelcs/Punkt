
#include <token/identifier_token.h>

#include "scanner.h"

Scanner::Scanner(LocatedCharStream *input_stream) {
    this->input_stream = input_stream;
}

Scanner::~Scanner() {
    delete this->input_stream;
}

Scanner *Scanner::MakeFromFileName(std::string file_name) {
    InputHandler *input_handler = new InputHandler(file_name);
    LocatedCharStream *input_stream = new LocatedCharStream(input_handler);
    return new Scanner(input_stream);
}

Token *Scanner::GetNextToken() {
    LocatedChar ch = this->input_stream->NextNonwhitespaceChar();

    if (ch.IsIdentifierStart()) {
        return ScanIdentifier(ch);
    }
    else {
        return GetNextToken();
    }
}

Token *Scanner::ScanIdentifier(LocatedChar first_char) {
    std::string buffer;
    buffer.push_back(first_char.character);
    LocatedChar ch = input_stream->Next();
    while (ch.IsIdentifierChar()) {
        buffer.push_back(ch.character);
        ch = input_stream->Next();
    }
    return new IdentifierToken(buffer, first_char.location);
}
