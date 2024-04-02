#include <token/all_tokens.h>
#include <logging/punkt_logger.h>

#include "scanner.h"
#include "keyword.h"
#include "punctuator_scanner.h"

Scanner::Scanner(fs::path file_path) : next_token(std::unique_ptr<EOFToken>()) {
    this->input_stream = std::make_unique<LocatedCharStream>(file_path);
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
    else if (ch.IsStringStart()) {
        return ScanString(ch);
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
        if (Keyword::ForLexeme(buffer) == KeywordEnum::TRUE ||
            Keyword::ForLexeme(buffer) == KeywordEnum::FALSE) {
            return std::make_unique<BooleanLiteralToken>(
                    buffer,
                    first_char.location,
                    Keyword::ForLexeme(buffer) == KeywordEnum::TRUE
            );
        }
        return std::make_unique<KeywordToken>(buffer, first_char.location, Keyword(buffer));
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

    switch (lc.character) {
        case '{':
        case '}':
        case '(':
        case ')':
        case ',':
        case '.':
        case '+':
        case '-':
        case '*':
        case '/':
            buffer.push_back(lc.character);
            break;

        case '=':
            buffer.push_back(lc.character);
            if (input_stream->Peek().character == '=' ||
                    input_stream->Peek().character == '<' ||
                    input_stream->Peek().character == '>') {
                buffer.push_back(input_stream->Next().character);
            }
            break;

        case '!':
        case '<':
        case '>':
            buffer.push_back(lc.character);
            if (input_stream->Peek().character == '=') {
                buffer.push_back(input_stream->Next().character);
            }
            break;

        default:
            PunktLogger::LogFatalInternalError("unexpected punctuator character");
    }
    return std::make_unique<PunctuatorToken>(
        buffer,
        first_char.location,
        Punctuator(buffer)
    );
    // return PunctuatorScanner::Scan(first_char, input_stream);
}

std::unique_ptr<Token> Scanner::ScanString(LocatedChar first_char)
{
    std::string buffer;
    ReadStringLiteral(buffer);

    LocatedChar next_char = input_stream->Peek();
    if (next_char.character != '\"') {
        LexicalErrorExpectedDifferentCharacter(next_char.character, '\"', next_char.location);
        return GetNextToken();
    }

    // Eat the closing double-quotes (")
    input_stream->Next();

    // Strings can span multiple lines, so continue reading if we can
    next_char = input_stream->NextNonwhitespaceChar();
    while (next_char.character == '\"') {
        ReadStringLiteral(buffer);
        next_char = input_stream->Peek();
        if (next_char.character != '\"') {
            LexicalErrorExpectedDifferentCharacter(next_char.character, '\"', next_char.location);
            return GetNextToken();
        }
        input_stream->Next();
        next_char = input_stream->NextNonwhitespaceChar();
    }

    // Once we are done, put the unused character back into the buffer for GetNextToken to read it
    input_stream->PutBack(next_char);

    return std::make_unique<StringLiteralToken>(buffer, first_char.location, buffer);
}

void Scanner::ReadStringLiteral(std::string& buffer)
{
    // Take a peek at the next character after double-quotes (")
    LocatedChar ch = input_stream->Peek();

    // Continue pushing back into the buffer until double-quotes (") or newline (\n)
    while (ch.character != '\"' && ch.character != '\n') {
        ch = input_stream->Next();
        buffer.push_back(ch.character);
        ch = input_stream->Peek();
    }
}

void Scanner::LexicalErrorUnexpectedCharacter(LocatedChar ch) {
    std::string message = "Unexpected character \'";
    message.push_back(ch.character);
    message.append("\' at ").append(ch.location.ToString());
    PunktLogger::Log(LogType::SCANNER, message);
}
void Scanner::LexicalErrorExpectedDifferentCharacter(char actual_char, char expected_char,
        TextLocation location) 
{
    std::string message = "Expected \'";
    message.push_back(expected_char);
    message += "\' at " + location.ToString();
    PunktLogger::Log(LogType::SCANNER, message);
}
