#ifndef SCANNER_H_
#define SCANNER_H_

#include <string>

#include <input_handler/located_char_stream.h>
#include <token/token.h>

class Scanner {
public:
    static Scanner *MakeFromFileName(std::string file_name);
    ~Scanner();

    Token *GetNextToken();

private:
    Scanner(LocatedCharStream *input_stream);

    Token *ScanIdentifier(LocatedChar first_char);

    LocatedCharStream *input_stream;
};

#endif // SCANNER_H_
