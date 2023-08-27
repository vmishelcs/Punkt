#ifndef PARSER_H_
#define PARSER_H_

#include <memory>

#include <scanner/scanner.h>
#include <parse_node/parse_node.h>

class Parser {
public:
    static std::unique_ptr<ParseNode> Parse(Scanner& scanner);

private:
    Parser(Scanner& scanner);

    void ReadToken();

    bool StartsProgram(Token& token);
    std::unique_ptr<ParseNode> ParseProgram();

    bool StartsMain(Token& token);
    std::unique_ptr<ParseNode> ParseMain();

    std::unique_ptr<ParseNode> GetSyntaxErrorNode(std::string expected);

    Scanner& scanner;
    std::unique_ptr<Token> now_reading;
};

#endif // PARSER_H_
