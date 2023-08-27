#ifndef PARSER_H_
#define PARSER_H_

#include "scanner/scanner.h"

class Parser {
public:
    Parser(std::unique_ptr<Scanner> scanner);

private:
    void ReadToken();

    std::unique_ptr<Scanner> scanner;
    std::unique_ptr<Token> now_reading;
};

#endif // PARSER_H_
