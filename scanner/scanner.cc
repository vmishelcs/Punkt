#include <token/identifier_token.h>
#include <token/integer_literal_token.h>
#include <token/keyword_token.h>
#include <token/punctuator_token.h>
#include <token/eof_token.h>
#include <logging/punkt_logger.h>

#include "scanner.h"
#include "keyword.h"
#include "punctuator_scanner.h"

Scanner::Scanner(std::string input_file_name) : next_token(std::make_shared<EOFToken>()) {
    this->input_stream = std::make_unique<LocatedCharStream>(input_file_name);
    this->next_token = GetNextToken();
}

std::shared_ptr<Token> Scanner::Next() {
    std::shared_ptr<Token> result = next_token;
    next_token = GetNextToken();
    return result;
}

bool Scanner::HasNext() const {
    return next_token->GetTokenType() != TokenType::EOF_TOKEN;
}

std::shared_ptr<Token> Scanner::GetNextToken() {
    LocatedChar ch = this->input_stream->NextNonwhitespaceChar();

    if (ch.IsIdentifierStart()) {
        return ScanIdentifier(ch);
    }
    else if (ch.IsNumberStart()) {
        return ScanNumber(ch);
    }
    else if (ch.IsPunctuatorChar()) {
        return ScanPunctuator(ch);
    }
    else if (IsEndOfInput(ch)) {
        return std::make_shared<EOFToken>();
    }
    else {
        LexicalErrorUnexpectedCharacter(ch);
        return GetNextToken();
    }
}

bool Scanner::IsEndOfInput(LocatedChar ch) {
    return ch == LocatedChar::EOF_LOCATED_CHAR;
}

std::shared_ptr<Token> Scanner::ScanIdentifier(LocatedChar first_char) {
    std::string buffer;
    buffer.push_back(first_char.character);
    LocatedChar ch = input_stream->Peek();
    while (ch.IsIdentifierChar()) {
        ch = input_stream->Next();
        buffer.push_back(ch.character);
        ch = input_stream->Peek();
    }

    if (Keyword::IsKeyword(buffer)) {
        Keyword keyword(buffer);
        return std::make_shared<KeywordToken>(buffer, first_char.location, std::move(keyword));
    }

    return std::make_shared<IdentifierToken>(buffer, first_char.location);
}

std::shared_ptr<Token> Scanner::ScanNumber(LocatedChar first_char) {
    std::string buffer;
    buffer.push_back(first_char.character);
    LocatedChar ch = input_stream->Peek();
    while (ch.IsDigit()) {
        ch = input_stream->Next();
        buffer.push_back(ch.character);
        ch = input_stream->Peek();
    }
    int value = std::stoi(buffer);
    return std::make_shared<IntegerLiteralToken>(buffer, first_char.location, value);
}

std::shared_ptr<Token> Scanner::ScanPunctuator(LocatedChar first_char) {
    return PunctuatorScanner::Scan(first_char, input_stream);
}

void Scanner::LexicalErrorUnexpectedCharacter(LocatedChar ch) {
    auto& logger = PunktLogger::GetInstance();
    std::string message = "Unexpected character \'";
    message.push_back(ch.character);
    message.append("\' at ").append(ch.GetLocationString());
    logger.Log(LogType::SCANNER, message);
}
