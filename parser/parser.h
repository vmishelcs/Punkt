#ifndef PARSER_H_
#define PARSER_H_

#include <memory>

#include <scanner/scanner.h>
#include <scanner/keyword.h>
#include <scanner/punctuator.h>
#include <parse_node/parse_node.h>

class Parser {
public:
    static std::unique_ptr<ParseNode> Parse(Scanner& scanner);

private:
    Parser(Scanner& scanner);

    void ReadToken();

    void Expect(KeywordEnum keyword);
    void Expect(PunctuatorEnum punctuator);

    bool StartsProgram(Token& token);
    std::unique_ptr<ParseNode> ParseProgram();

    bool StartsMain(Token& token);
    std::unique_ptr<ParseNode> ParseMain();

    bool StartsStatement(Token& token);
    std::unique_ptr<ParseNode> ParseStatement();

    bool StartsDeclaration(Token& token);
    std::unique_ptr<ParseNode> ParseDeclaration();

    bool StartsIdentifier(Token& token);
    std::unique_ptr<ParseNode> ParseIdentifier();

    bool StartsExpression(Token& token);
    std::unique_ptr<ParseNode> ParseExpression();

    bool StartsAdditiveExpression(Token& token);
    std::unique_ptr<ParseNode> ParseAdditiveExpression();

    bool StartsMultiplicativeExpression(Token& token);
    std::unique_ptr<ParseNode> ParseMultiplicativeExpression();

    bool StartsUnaryExpression(Token& token);
    std::unique_ptr<ParseNode> ParseUnaryExpression();

    bool StartsAtomicExpression(Token& token);
    std::unique_ptr<ParseNode> ParseAtomicExpression();

    bool StartsParenthesizedExpression(Token& token);
    std::unique_ptr<ParseNode> ParseParenthesizedExpression();

    bool StartsIntegerLiteral(Token& token);
    std::unique_ptr<ParseNode> ParseIntegerLiteral();

    std::unique_ptr<ParseNode> SyntaxErrorUnexpectedToken(std::string expected);
    std::unique_ptr<ParseNode> GetSyntaxErrorNode();

    Scanner& scanner;
    std::unique_ptr<Token> now_reading;
};

#endif // PARSER_H_
