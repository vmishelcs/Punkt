#include "scanner.h"

#include <input_handler/text_location.h>
#include <logging/punkt_logger.h>
#include <token/all_tokens.h>

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <string>

#include "keyword.h"

static const size_t kMaxIdentifierLength = 32;

static void IdentifierTooLongError(const TextLocation &, const std::string &);
static void UnexpectedCharacterError(const TextLocation &, char);
static void ExpectedDifferentCharacterError(const TextLocation &, char);

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
    return ScanKeywordOrIdentifier(ch);
  } else if (ch.IsNumberStart()) {
    return ScanNumber(ch);
  } else if (ch.IsOperatorOrPunctuatorStart()) {
    return ScanOperatorOrPunctuator(ch);
  } else if (ch.IsCharacterStart()) {
    return ScanCharacter(ch);
  } else if (ch.IsStringStart()) {
    return ScanString(ch);
  } else if (ch.IsCommentStart()) {
    ch = this->input_stream->Next();
    while (ch.IsPartOfComment() && !IsEndOfInput(ch)) {
      ch = this->input_stream->Next();
    }
    return GetNextToken();
  } else if (IsEndOfInput(ch)) {
    return std::make_unique<EOFToken>();
  } else {
    UnexpectedCharacterError(ch.location, ch.character);
    return GetNextToken();
  }
}

bool Scanner::IsEndOfInput(LocatedChar ch) {
  return ch == LocatedChar::EOF_LOCATED_CHAR;
}

std::unique_ptr<Token> Scanner::ScanKeywordOrIdentifier(
    LocatedChar first_char) {
  std::string buffer;
  buffer.push_back(first_char.character);
  LocatedChar ch = input_stream->Peek();
  while (ch.IsIdentifierChar()) {
    ch = input_stream->Next();
    buffer.push_back(ch.character);
    ch = input_stream->Peek();
  }

  if (keyword_utils::IsKeyword(buffer)) {
    Keyword keyword = keyword_utils::GetKeywordEnum(buffer);
    if (keyword == Keyword::TRUE || keyword == Keyword::FALSE) {
      return std::make_unique<BooleanLiteralToken>(buffer, first_char.location,
                                                   keyword == Keyword::TRUE);
    }
    if (keyword == Keyword::ALLOC) {
      return std::make_unique<OperatorToken>(buffer, first_char.location,
                                             Operator::ALLOC);
    }
    if (keyword == Keyword::SIZEOF) {
      return std::make_unique<OperatorToken>(buffer, first_char.location,
                                             Operator::SIZEOF);
    }
    return std::make_unique<KeywordToken>(buffer, first_char.location, keyword);
  }

  if (buffer.size() > kMaxIdentifierLength) {
    IdentifierTooLongError(first_char.location, buffer);
    return GetNextToken();
  }

  return std::make_unique<IdentifierToken>(buffer, first_char.location);
}

std::unique_ptr<Token> Scanner::ScanNumber(LocatedChar first_char) {
  // Append any subsequent digits to the buffer.
  std::string buffer;
  buffer.push_back(first_char.character);
  LocatedChar ch = input_stream->Peek();
  while (ch.IsDigit()) {
    ch = input_stream->Next();
    buffer.push_back(ch.character);
    ch = input_stream->Peek();
  }

  LocatedChar next = input_stream->Next();
  LocatedChar second_next = input_stream->Peek();
  input_stream->PutBack(next);

  // Check if we are scanning a floating point number.
  if (next.character == '.' && second_next.IsDigit()) {
    // Scan the decimal part of the floating point number.
    next = input_stream->Next();
    buffer.push_back(next.character);
    next = input_stream->Peek();
    while (next.IsDigit()) {
      next = input_stream->Next();
      buffer.push_back(next.character);
      next = input_stream->Peek();
    }

    next = input_stream->Next();
    second_next = input_stream->Peek();

    // Check for scientific notation.
    if (next.character == 'e' || next.character == 'E') {
      // Next character following 'E' must be either a digit or a numeric sign.
      if (second_next.character != '+' && second_next.character != '-' &&
          !second_next.IsDigit()) {
        UnexpectedCharacterError(second_next.location, second_next.character);
        return GetNextToken();
      }

      // Append 'E' to the buffer.
      buffer.push_back(next.character);

      // Check for numeric sign.
      if (second_next.character == '+' || second_next.character == '-') {
        // Make sure there are digits after the numeric sign.
        next = input_stream->Next();
        second_next = input_stream->Peek();
        if (!second_next.IsDigit()) {
          UnexpectedCharacterError(second_next.location, second_next.character);
          return GetNextToken();
        }

        // Append the numeric sign.
        buffer.push_back(next.character);
      }

      // Append any subsequent digits.
      next = input_stream->Peek();
      while (next.IsDigit()) {
        next = input_stream->Next();
        buffer.push_back(next.character);
        next = input_stream->Peek();
      }
    } else {
      // If we did not find 'e' or 'E', put whatever character was in `next`
      // back into the input stream.
      input_stream->PutBack(next);
    }

    double value = strtod(buffer.c_str(), nullptr);
    // Check for underflow/overflow.
    if (errno == ERANGE) {
      PunktLogger::LogCompileError(first_char.location,
                                   "floating point literal cannot be "
                                   "represented as a double-precision float");
      errno = 0;  // Reset `errno`.
      return GetNextToken();
    }
    return std::make_unique<FloatLiteralToken>(buffer, first_char.location,
                                               value);
  }

  int64_t value = strtoll(buffer.c_str(), nullptr, 10);
  // Check for overflow.
  if (errno == ERANGE) {
    PunktLogger::LogCompileError(
        first_char.location,
        "integer literal is too large to be represented as a 64-bit integer");
    errno = 0;  // Reset `errno`.
    return GetNextToken();
  }
  return std::make_unique<IntegerLiteralToken>(buffer, first_char.location,
                                               value);
}

std::unique_ptr<Token> Scanner::ScanOperatorOrPunctuator(
    LocatedChar first_char) {
  std::string buffer;
  LocatedChar lc = first_char;

  switch (lc.character) {
    case '{':
    case '}':
    case '[':
    case ']':
    case '(':
    case ')':
    case ',':
    case '.':
      buffer.push_back(lc.character);
      break;
    case '+':
    case '*':
    case '%':
      buffer.push_back(lc.character);
      if (input_stream->Peek().character == '=') {
        // += (add and assign) operator
        // *= (multiply and assign) operator
        // %= (mod and assign) operator
        buffer.push_back(input_stream->Next().character);
      }
      break;

    case '/':
      buffer.push_back(lc.character);
      if (input_stream->Peek().character == '=' ||
          input_stream->Peek().character == '/') {
        // '/=' (divide and assign) operator
        // '//' (over) operator
        buffer.push_back(input_stream->Next().character);
      }
      break;

    case '-':
      buffer.push_back(lc.character);
      if (input_stream->Peek().character == '>' ||
          input_stream->Peek().character == '=') {
        // '->' (returns) punctuator
        // '-=' (subtract and assign) operator
        buffer.push_back(input_stream->Next().character);
      }
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

    case '&':
      buffer.push_back(lc.character);
      if (input_stream->Peek().character == '&') {
        buffer.push_back(input_stream->Next().character);
      } else {
        UnexpectedCharacterError(input_stream->Peek().location,
                                 input_stream->Peek().character);
        return GetNextToken();
      }
      break;

    case '|':
      buffer.push_back(lc.character);
      if (input_stream->Peek().character == '|') {
        buffer.push_back(input_stream->Next().character);
      } else {
        UnexpectedCharacterError(input_stream->Peek().location,
                                 input_stream->Peek().character);
        return GetNextToken();
      }
      break;

    default:
      PunktLogger::LogFatalInternalError(
          "unexpected character encountered when scanning operator/punctuator");
  }

  if (operator_utils::IsOperator(buffer)) {
    Operator operator_enum = operator_utils::GetOperatorEnum(buffer);
    return std::make_unique<OperatorToken>(buffer, first_char.location,
                                           operator_enum);
  }

  if (punctuator_utils::IsPunctuator(buffer)) {
    Punctuator punctuator_enum = punctuator_utils::GetPunctuatorEnum(buffer);
    return std::make_unique<PunctuatorToken>(buffer, first_char.location,
                                             punctuator_enum);
  }

  PunktLogger::LogFatalInternalError("unrecognizable operator/punctuator");
  return GetNextToken();
}

std::unique_ptr<Token> Scanner::ScanCharacter(LocatedChar first_char) {
  LocatedChar ch = input_stream->Next();
  char char_literal_value = '\0';

  // If no character is specified, create a CharacterLiteralToken denoting a
  // null char.
  if (ch.character == '\'') {
    return std::make_unique<CharacterLiteralToken>("", ch.location,
                                                   char_literal_value);
  }

  if (ch.IsEscapeSequenceStart()) {
    char_literal_value = InterpretEscapeSequence();
  } else {
    char_literal_value = ch.character;
  }

  LocatedChar next_char = input_stream->Next();
  if (next_char.character != '\'') {
    ExpectedDifferentCharacterError(next_char.location, next_char.character);
    return GetNextToken();
  }

  return std::make_unique<CharacterLiteralToken>(
      std::to_string(char_literal_value), ch.location, char_literal_value);
}

std::unique_ptr<Token> Scanner::ScanString(LocatedChar first_char) {
  std::string buffer;
  ReadStringLiteral(buffer);

  LocatedChar next_char = input_stream->Peek();
  if (next_char.character != '\"') {
    ExpectedDifferentCharacterError(next_char.location, next_char.character);
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
      ExpectedDifferentCharacterError(next_char.location, next_char.character);
      return GetNextToken();
    }
    input_stream->Next();
    next_char = input_stream->NextNonwhitespaceChar();
  }

  // Once we are done, put the unused character back into the buffer for
  // GetNextToken to read it
  input_stream->PutBack(next_char);

  return std::make_unique<StringLiteralToken>(buffer, first_char.location,
                                              buffer);
}

void Scanner::ReadStringLiteral(std::string &buffer) {
  // Take a peek at the next character after double-quotes (")
  LocatedChar ch = input_stream->Peek();

  // Continue pushing back into the buffer until double-quotes (") or newline
  // (\n)
  while (ch.character != '\"' && ch.character != '\n') {
    ch = input_stream->Next();
    if (ch.IsEscapeSequenceStart()) {
      buffer.push_back(InterpretEscapeSequence());
    } else {
      buffer.push_back(ch.character);
    }
    ch = input_stream->Peek();
  }
}

char Scanner::InterpretEscapeSequence() {
  // Get the character following the metacharacter '\'.
  LocatedChar ch = input_stream->Next();
  switch (ch.character) {
    case 'a':
      // Bell
      return '\a';
    case 'b':
      // Backspace
      return '\b';
    case 'f':
      // Formfeed page break
      return '\f';
    case 'n':
      // Newline
      return '\n';
    case 'r':
      // Carriage return
      return '\r';
    case 't':
      // Horizontal tab
      return '\t';
    case 'v':
      // Vertical tab
      return '\v';
    case '0':
      // Null char
      return '\0';
    case '\\':
      // Backslash
      return '\\';
    case '\'':
      // Apostrophe
      return '\'';
    case '\"':
      // Double quotes
      return '\"';
    default:
      // In other cases, just return the character as is.
      return ch.character;
  }
}

//===----------------------------------------------------------------------===//
// Error handling
//===----------------------------------------------------------------------===//
void IdentifierTooLongError(const TextLocation &text_location,
                            const std::string &id_name) {
  PunktLogger::LogCompileError(
      text_location, "variable identifier \'" + id_name + "\' is too long");
}

void UnexpectedCharacterError(const TextLocation &text_location, char c) {
  std::string message = "unexpected character \'";
  message.push_back(c);
  message.push_back('\'');
  PunktLogger::LogCompileError(text_location, message);
}

void ExpectedDifferentCharacterError(const TextLocation &text_location,
                                     char c) {
  std::string message = "expected character \'";
  message.push_back(c);
  message.push_back('\'');
  PunktLogger::LogCompileError(text_location, message);
}
