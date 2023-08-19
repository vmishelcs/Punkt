#include <token/identifier_token.h>
#include <token/integer_literal_token.h>
#include <logging/punkt_logger.h>

#include "scanner.h"

Scanner::Scanner(std::string input_file_name) {
    this->input_stream = std::make_unique<LocatedCharStream>(input_file_name);
}

std::shared_ptr<Token> Scanner::GetNextToken() {
    LocatedChar ch = this->input_stream->NextNonwhitespaceChar();

    if (ch.IsIdentifierStart()) {
        return ScanIdentifier(ch);
    }
    else if (ch.IsNumberStart()) {
        return ScanNumber(ch);
    }
    else if (ch.IsEndOfInput()) {
        // TODO: Create an EOF token here
        return nullptr;
    }
    else {
        LexicalErrorUnexpectedCharacter(ch);
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

std::shared_ptr<Token> Scanner::ScanNumber(LocatedChar first_char) {
    std::string buffer;
    buffer.push_back(first_char.character);
    LocatedChar ch = input_stream->Next();
    while (ch.IsDigit()) {
        buffer.push_back(ch.character);
        ch = input_stream->Next();
    }
    int value = std::stoi(buffer);
    return std::make_shared<IntegerLiteralToken>(buffer, value, first_char.location);
}

void Scanner::LexicalErrorUnexpectedCharacter(LocatedChar ch) {
    auto& logger = PunktLogger::GetInstance();
    std::string message = "Unexpected character `";
    message.push_back(ch.character);
    message.append("` at ").append(ch.GetLocationString());
    logger.Log(LogType::SCANNER, message);
}
