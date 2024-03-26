#include <glog/logging.h>

#include <token/all_tokens.h>
#include <logging/punkt_logger.h>

#include "scanner.h"
#include "keyword.h"
#include "punctuator_scanner.h"

Scanner::Scanner(std::string input_file_name) : next_token(std::unique_ptr<EOFToken>()) {
    this->input_stream = std::make_unique<LocatedCharStream>(input_file_name);
    this->next_token = GetNextToken();
}

std::unique_ptr<Token> Scanner::Next() {
    std::unique_ptr<Token> result = std::move(next_token);
    next_token = GetNextToken();
    return result;
}

bool Scanner::HasNext() const {
    return next_token->GetTokenType() != TokenType::EOF_TOKEN;
}

std::unique_ptr<Token> Scanner::GetNextToken() {
    LocatedChar ch = this->input_stream->NextNonwhitespaceChar();

    if (ch.IsIdentifierStart()) {
        return ScanIdentifier(ch);
    }
    else if (ch.IsNumberStart()) {
        return ScanNumber(ch);
    }
    else if (ch.IsPunctuatorStart()) {
        return ScanPunctuator(ch);
    }
    else if (IsEndOfInput(ch)) {
        return std::make_unique<EOFToken>();
    }
    else {
        LexicalErrorUnexpectedCharacter(ch);
        return GetNextToken();
    }
}

bool Scanner::IsEndOfInput(LocatedChar ch) {
    return ch == LocatedChar::EOF_LOCATED_CHAR;
}

std::unique_ptr<Token> Scanner::ScanIdentifier(LocatedChar first_char) {
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
        return std::make_unique<KeywordToken>(buffer, first_char.location, std::move(keyword));
    }

    return std::make_unique<IdentifierToken>(buffer, first_char.location);
}

std::unique_ptr<Token> Scanner::ScanNumber(LocatedChar first_char) {
    std::string buffer;
    buffer.push_back(first_char.character);
    LocatedChar ch = input_stream->Peek();
    while (ch.IsDigit()) {
        ch = input_stream->Next();
        buffer.push_back(ch.character);
        ch = input_stream->Peek();
    }
    int value = std::stoi(buffer);
    return std::make_unique<IntegerLiteralToken>(buffer, first_char.location, value);
}

std::unique_ptr<Token> Scanner::ScanPunctuator(LocatedChar first_char) {
    std::string buffer;
    LocatedChar lc = first_char;

    // These switch cases may get more complex with addition of new punctuators.
    switch (lc.character) {
        case '{':
            buffer.push_back(lc.character);
            break;
            
        case '}':
            buffer.push_back(lc.character);
            break;
            
        case '(':
            buffer.push_back(lc.character);
            break;
            
        case ')':
            buffer.push_back(lc.character);
            break;
            
        case ',':
            buffer.push_back(lc.character);
            break;
            
        case '.':
            buffer.push_back(lc.character);
           break;
            
        case '=':
            buffer.push_back(lc.character);
            break;
            
        case '+':
            buffer.push_back(lc.character);
            break;
            
        case '-':
            buffer.push_back(lc.character);
            break;
            
        case '*':
            buffer.push_back(lc.character);
            break;
            
        case '/':
            buffer.push_back(lc.character);
            break;

        default:
            LOG(FATAL) << "Internal error - unexpected punctuator character";
    }
    return std::make_unique<PunctuatorToken>(
        buffer,
        first_char.location,
        Punctuator(buffer)
    );
    // return PunctuatorScanner::Scan(first_char, input_stream);
}

void Scanner::LexicalErrorUnexpectedCharacter(LocatedChar ch) {
    auto& logger = PunktLogger::GetInstance();
    std::string message = "Unexpected character \'";
    message.push_back(ch.character);
    message.append("\' at ").append(ch.location.ToString());
    logger.Log(LogType::SCANNER, message);
}
