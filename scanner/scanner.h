#ifndef SCANNER_H_
#define SCANNER_H_

#include <memory>
#include <string>

#include <input_handler/located_char_stream.h>
#include <token/token.h>

class Scanner {
public:
    Scanner(std::string input_file_name);

    std::shared_ptr<Token> Next();
    bool HasNext() const;

private:
    std::shared_ptr<Token> GetNextToken();
    std::shared_ptr<Token> ScanIdentifier(LocatedChar first_char);
    std::shared_ptr<Token> ScanNumber(LocatedChar first_char);
    std::shared_ptr<Token> ScanPunctuator(LocatedChar first_char);

    bool IsEndOfInput(LocatedChar ch);

    void LexicalErrorUnexpectedCharacter(LocatedChar ch);

    std::unique_ptr<LocatedCharStream> input_stream;
    std::shared_ptr<Token> next_token;
};

#endif // SCANNER_H_
