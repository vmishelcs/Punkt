#ifndef PARSER_H_
#define PARSER_H_

#include <parse_node/parse_node.h>
#include <scanner/keyword.h>
#include <scanner/punctuator.h>
#include <scanner/scanner.h>

class Parser {
 public:
  static std::unique_ptr<ParseNode> Parse(fs::path file_path);

 private:
  Parser(std::unique_ptr<Scanner> scanner);

  void ReadToken();

  void Expect(KeywordEnum keyword);
  void Expect(Punctuator punctuator);

  bool StartsProgram(Token &token);
  std::unique_ptr<ParseNode> ParseProgram();

  bool StartsFunctionDefinition(Token &token);
  std::unique_ptr<ParseNode> ParseFunctionDefinition();

  bool StartsLambda(Token &token);
  std::unique_ptr<ParseNode> ParseLambda();

  bool StartsMain(Token &token);
  std::unique_ptr<ParseNode> ParseMain();

  bool StartsStatement(Token &token);
  std::unique_ptr<ParseNode> ParseStatement();

  bool StartsCodeBlock(Token &token);
  std::unique_ptr<ParseNode> ParseCodeBlock();

  /// declarationStatement ::- `var` identifierToken `=` expression.
  ///                       |  `const` identifierToken `=` expression.
  bool StartsDeclaration(Token &token);
  std::unique_ptr<ParseNode> ParseDeclaration(bool expect_terminator = true);

  /// expressionStatement ::- expression.
  bool StartsExpressionStatement(Token &token);
  std::unique_ptr<ParseNode> ParseExpressionStatement(
      bool expect_terminator = true);

  bool StartsIfStatement(Token &token);
  bool StartsElseBlock(Token &token);
  std::unique_ptr<ParseNode> ParseIfStatement();

  bool StartsForStatement(Token &token);
  std::unique_ptr<ParseNode> ParseForStatement();

  bool StartsCallStatement(Token &token);
  std::unique_ptr<ParseNode> ParseCallStatement();

  bool StartsPrintStatement(Token &token);
  std::unique_ptr<ParseNode> ParsePrintStatement(bool expect_terminator = true);

  bool StartsPrintExpressionList(Token &token);
  std::unique_ptr<ParseNode> ParsePrintExpressionList(
      std::unique_ptr<ParseNode> print_statement);

  bool StartsReturnStatement(Token &token);
  std::unique_ptr<ParseNode> ParseReturnStatement();

  bool StartsExpression(Token &token);
  std::unique_ptr<ParseNode> ParseExpression();

  /// `=` Simple assignment
  /// `+=` `-=` Assignment by addition and subtraction (not implemented)
  /// `*=` `/=` `%=` Assignment by multiplication, division, and modulus (not
  /// implemented)
  bool StartsAssignmentExpression(Token &token);
  std::unique_ptr<ParseNode> ParseAssignmentExpression();

  /// `==` `!=` Relational equality and inequality
  bool StartsEqualityExpression(Token &token);
  std::unique_ptr<ParseNode> ParseEqualityExpression();

  /// `<` `<=` Relational less than and less than or equal to
  /// `>` `>=` Relational greater than and greater than or equal to
  bool StartsComparisonExpression(Token &token);
  std::unique_ptr<ParseNode> ParseComparisonExpression();

  /// `+` `-` Addition and subtraction
  bool StartsAdditiveExpression(Token &token);
  std::unique_ptr<ParseNode> ParseAdditiveExpression();

  /// `*` `/` `%` Multiplication, division, and modulus (not implemented)
  bool StartsMultiplicativeExpression(Token &token);
  std::unique_ptr<ParseNode> ParseMultiplicativeExpression();

  bool StartsUnaryExpression(Token &token);
  std::unique_ptr<ParseNode> ParseUnaryExpression();

  bool StartsAtomicExpression(Token &token);
  std::unique_ptr<ParseNode> ParseAtomicExpression();

  bool StartsParenthesizedExpression(Token &token);
  std::unique_ptr<ParseNode> ParseParenthesizedExpression();

  bool StartsIdentifier(Token &token);
  std::unique_ptr<ParseNode> ParseIdentifier();

  bool StartsBooleanLiteral(Token &token);
  std::unique_ptr<ParseNode> ParseBooleanLiteral();

  bool StartsCharacterLiteral(Token &token);
  std::unique_ptr<ParseNode> ParseCharacterLiteral();

  bool StartsIntegerLiteral(Token &token);
  std::unique_ptr<ParseNode> ParseIntegerLiteral();

  bool StartsStringLiteral(Token &token);
  std::unique_ptr<ParseNode> ParseStringLiteral();

  bool StartsLambdaLiteral(Token &token);
  std::unique_ptr<ParseNode> ParseLambdaLiteral();

  bool StartsType(Token &token);
  std::unique_ptr<ParseNode> ParseType();

  bool StartsBaseType(Token &token);
  std::unique_ptr<ParseNode> ParseBaseType();

  bool StartsLambdaType(Token &token);
  std::unique_ptr<ParseNode> ParseLambdaType();

  bool StartsLambdaInvocation(Token &token);
  std::unique_ptr<ParseNode> ParseLambdaInvocation(
      std::unique_ptr<ParseNode> lambda);

  std::unique_ptr<ParseNode> SyntaxErrorUnexpectedToken(std::string expected);
  std::unique_ptr<ParseNode> GetSyntaxErrorNode();

  std::unique_ptr<Scanner> scanner;
  std::unique_ptr<Token> now_reading;
};

#endif  // PARSER_H_
