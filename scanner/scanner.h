#ifndef SCANNER_H_
#define SCANNER_H_

#include <memory>
#include <string>

#include <input_handler/located_char_stream.h>
#include <token/token.h>

class Scanner {
public:
    Scanner(std::string input_file_name);

    std::shared_ptr<Token> GetNextToken();

private:
    std::shared_ptr<Token> ScanIdentifier(LocatedChar first_char);

    std::unique_ptr<LocatedCharStream> input_stream;
};

#endif // SCANNER_H_
