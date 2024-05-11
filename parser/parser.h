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

  void Expect(Keyword keyword);
  void Expect(Operator op);
  void Expect(Punctuator punctuator);

  bool StartsProgram(Token &token);
  std::unique_ptr<ParseNode> ParseProgram();

  bool StartsFunctionDefinition(Token &token);
  std::unique_ptr<ParseNode> ParseFunctionDefinition();

  bool StartsLambda(Token &token);
  std::unique_ptr<ParseNode> ParseLambda();

  bool StartsMain(Token &token);
  std::unique_ptr<ParseNode> ParseMain();

  /// <stmt> ::= <declStmt>
  ///         |  <exprStmt>
  ///         |  <returnStmt>
  ///         |  <ifStmt>
  ///         |  <whileStmt>
  ///         |  <forStmt>
  ///         |  <printStmt>
  ///         |  <codeBlock>
  bool StartsStatement(Token &token);
  std::unique_ptr<ParseNode> ParseStatement();

  /// <codeBlock> ::= { <stmt>* }
  bool StartsCodeBlock(Token &token);
  std::unique_ptr<ParseNode> ParseCodeBlock();

  /// <declStmt> ::= var <identifierToken> = <expression> .
  ///             |  const <identifierToken> = <expression> .
  bool StartsDeclaration(Token &token);
  std::unique_ptr<ParseNode> ParseDeclaration(bool expect_terminator = true);

  /// <exprStmt> ::= <expr> .
  bool StartsExpressionStatement(Token &token);
  std::unique_ptr<ParseNode> ParseExpressionStatement(
      bool expect_terminator = true);

  /// <ifStmt> ::= if <expression> (<stmt> | <codeBlock>) <elsePart>
  /// <elsePart> ::= else (<stmt> | <codeBlock>)
  ///             |  _
  bool StartsIfStatement(Token &token);
  bool StartsElseBlock(Token &token);
  std::unique_ptr<ParseNode> ParseIfStatement();

  /// <whileStmt> ::= while <expression> (<stmt> | <codeBlock>)
  bool StartsWhileStatement(Token &token);
  std::unique_ptr<ParseNode> ParseWhileStatement();

  /// <forStmt> ::= for (<declStmt> | <expr>) , <expr> , <expr> <codeBlock>
  bool StartsForStatement(Token &token);
  std::unique_ptr<ParseNode> ParseForStatement();

  bool StartsCallStatement(Token &token);
  std::unique_ptr<ParseNode> ParseCallStatement();

  bool StartsPrintStatement(Token &token);
  std::unique_ptr<ParseNode> ParsePrintStatement();

  /// <returnStmt> ::= return <returnValue> .
  /// <returnValue> ::= <expr>
  ///                |  _
  bool StartsReturnStatement(Token &token);
  std::unique_ptr<ParseNode> ParseReturnStatement();

  bool StartsExpression(Token &token);
  std::unique_ptr<ParseNode> ParseExpression();

  /// `=` Simple assignment
  /// `+=` `-=` Assignment by addition and subtraction
  /// `*=` `/=` `%=` Assignment by multiplication, division, and modulo
  bool StartsAssignmentExpression(Token &token);
  std::unique_ptr<ParseNode> ParseAssignmentExpression();

  /// `||` Boolean OR
  bool StartsBooleanORExpression(Token &token);
  std::unique_ptr<ParseNode> ParseBooleanORExpression();

  /// `&&` Boolean AND
  bool StartsBooleanANDExpression(Token &token);
  std::unique_ptr<ParseNode> ParseBooleanANDExpression();

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

  /// `*` `/` `%` Multiplication, division, and modulo
  bool StartsMultiplicativeExpression(Token &token);
  std::unique_ptr<ParseNode> ParseMultiplicativeExpression();

  /// `!` Boolean negation
  /// `+` `-` Unary integer plus and minus
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

  bool StartsAllocExpression(Token &token);
  std::unique_ptr<ParseNode> ParseAllocExpression();

  bool StartsLambdaLiteral(Token &token);
  std::unique_ptr<ParseNode> ParseLambdaLiteral();

  bool StartsType(Token &token);
  std::unique_ptr<ParseNode> ParseType();

  bool StartsBaseType(Token &token);
  std::unique_ptr<ParseNode> ParseBaseType();

  bool StartsArrayType(Token &token);
  std::unique_ptr<ParseNode> ParseArrayType();

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
