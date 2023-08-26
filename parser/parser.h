#ifndef PARSER_H_
#define PARSER_H_

#include "scanner/scanner.h"

class Parser {
public:
    Parser(Scanner& scanner);

private:
    Scanner& scanner;
};

#endif // PARSER_H_
