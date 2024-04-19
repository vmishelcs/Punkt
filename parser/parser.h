#ifndef PARSER_H_
#define PARSER_H_

#include <scanner/scanner.h>
#include <scanner/keyword.h>
#include <scanner/punctuator.h>
#include <parse_node/parse_node.h>

class Parser {
public:
    static std::unique_ptr<ParseNode> Parse(fs::path file_path);

private:
    Parser(std::unique_ptr<Scanner> scanner);

    void ReadToken();

    void Expect(KeywordEnum keyword);
    void Expect(PunctuatorEnum punctuator);

    bool StartsProgram(Token& token);
    std::unique_ptr<ParseNode> ParseProgram();

    bool StartsFunctionDefinition(Token& token);
    std::unique_ptr<ParseNode> ParseFunctionDefinition();

    bool StartsLambda(Token& token);
    std::unique_ptr<ParseNode> ParseLambda();

    bool StartsMain(Token& token);
    std::unique_ptr<ParseNode> ParseMain();

    bool StartsStatement(Token& token);
    std::unique_ptr<ParseNode> ParseStatement();

    bool StartsCodeBlock(Token& token);
    std::unique_ptr<ParseNode> ParseCodeBlock();

    bool StartsDeclaration(Token& token);
    std::unique_ptr<ParseNode> ParseDeclaration(bool expect_terminator = true);

    bool StartsAssignment(Token& token);
    std::unique_ptr<ParseNode> ParseAssignment(bool expect_terminator = true);

    bool StartsTargettableExpression(Token& token);
    std::unique_ptr<ParseNode> ParseTargettableExpression();

    bool StartsIfStatement(Token& token);
    bool StartsElseBlock(Token& token);
    std::unique_ptr<ParseNode> ParseIfStatement();

    bool StartsForStatement(Token& token);
    std::unique_ptr<ParseNode> ParseForStatement();

    bool StartsPrintStatement(Token& token);
    std::unique_ptr<ParseNode> ParsePrintStatement(bool expect_terminator = true);

    bool StartsPrintExpressionList(Token& token);
    std::unique_ptr<ParseNode> ParsePrintExpressionList(std::unique_ptr<ParseNode> print_statement);

    bool StartsReturnStatement(Token& token);
    std::unique_ptr<ParseNode> ParseReturnStatement();

    bool StartsExpression(Token& token);
    std::unique_ptr<ParseNode> ParseExpression();

    bool StartsBooleanExpression(Token& token);
    std::unique_ptr<ParseNode> ParseBooleanExpression();

    bool StartsEqualityExpression(Token& token);
    std::unique_ptr<ParseNode> ParseEqualityExpression();

    bool StartsComparisonExpression(Token& token);
    std::unique_ptr<ParseNode> ParseComparisonExpression();

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

    bool StartsIdentifier(Token& token);
    std::unique_ptr<ParseNode> ParseIdentifier();

    bool StartsBooleanLiteral(Token& token);
    std::unique_ptr<ParseNode> ParseBooleanLiteral();

    bool StartsCharacterLiteral(Token& token);
    std::unique_ptr<ParseNode> ParseCharacterLiteral();

    bool StartsIntegerLiteral(Token& token);
    std::unique_ptr<ParseNode> ParseIntegerLiteral();

    bool StartsStringLiteral(Token& token);
    std::unique_ptr<ParseNode> ParseStringLiteral();

    bool StartsLambdaLiteral(Token& token);
    std::unique_ptr<ParseNode> ParseLambdaLiteral();

    bool StartsType(Token& token);
    std::unique_ptr<ParseNode> ParseType();

    bool StartsBaseType(Token& token);
    std::unique_ptr<ParseNode> ParseBaseType();

    bool StartsLambdaType(Token& token);
    std::unique_ptr<ParseNode> ParseLambdaType();

    bool StartsLambdaInvocation(Token& token);
    std::unique_ptr<ParseNode> ParseLambdaInvocation(std::unique_ptr<ParseNode> lambda);

    std::unique_ptr<ParseNode> SyntaxErrorUnexpectedToken(std::string expected);
    std::unique_ptr<ParseNode> GetSyntaxErrorNode();

    std::unique_ptr<Scanner> scanner;
    std::unique_ptr<Token> now_reading;
};

#endif // PARSER_H_
