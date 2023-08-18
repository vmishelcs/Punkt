
#include <token/identifier_token.h>

#include "scanner.h"

Scanner::Scanner(std::string input_file_name) {
    this->input_stream = std::make_unique<LocatedCharStream>(input_file_name);
}

std::shared_ptr<Token> Scanner::GetNextToken() {
    LocatedChar ch = this->input_stream->NextNonwhitespaceChar();

    if (ch.IsIdentifierStart()) {
        return ScanIdentifier(ch);
    }
    else {
        return GetNextToken();
    }
}

std::shared_ptr<Token> Scanner::ScanIdentifier(LocatedChar first_char) {
    std::string buffer;
    buffer.push_back(first_char.character);
    LocatedChar ch = input_stream->Next();
    while (ch.IsIdentifierChar()) {
        buffer.push_back(ch.character);
        ch = input_stream->Next();
    }
    return std::make_shared<IdentifierToken>(buffer, first_char.location);
}
