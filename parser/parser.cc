#include "parser.h"

#include <logging/punkt_logger.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <scanner/scanner.h>
#include <token/all_tokens.h>

#include <memory>
#include <vector>

std::unique_ptr<ParseNode> Parser::Parse(fs::path file_path) {
  std::unique_ptr<Scanner> scanner = std::make_unique<Scanner>(file_path);
  Parser parser(std::move(scanner));
  auto ast = parser.ParseProgram();

  // TODO: This cast causes an exception on bad input.
  // Example:
  // main { else {} }
  ProgramNode *program_node = dynamic_cast<ProgramNode *>(ast.get());
  if (!program_node) {
    return parser.GetSyntaxErrorNode();
  }
  program_node->SetModuleID(file_path.string());

  return ast;
}

Parser::Parser(std::unique_ptr<Scanner> scanner) : scanner(std::move(scanner)) {
  ReadToken();
}

void Parser::ReadToken() {
  now_reading = scanner->Next();
  if (!now_reading) {
    PunktLogger::LogFatalInternalError(
        "failed to read token in Parser::ReadToken");
  }
}

void Parser::Expect(KeywordEnum keyword) {
  if (now_reading->GetTokenType() != TokenType::KEYWORD) {
    SyntaxErrorUnexpectedToken("\'" + Keyword::ForKeywordEnum(keyword) + "\'");
    ReadToken();
    return;
  }

  KeywordToken &keyword_token = dynamic_cast<KeywordToken &>(*now_reading);
  if (keyword_token.GetKeywordEnum() != keyword) {
    SyntaxErrorUnexpectedToken("\'" + Keyword::ForKeywordEnum(keyword) + "\'");
    ReadToken();
    return;
  }

  ReadToken();
}

void Parser::Expect(PunctuatorEnum punctuator) {
  if (now_reading->GetTokenType() != TokenType::PUNCTUATOR) {
    SyntaxErrorUnexpectedToken(
        "\'" + Punctuator::ForPunctuatorEnum(punctuator) + "\'");
    ReadToken();
    return;
  }

  PunctuatorToken &punctuator_token =
      dynamic_cast<PunctuatorToken &>(*now_reading);
  if (punctuator_token.GetPunctuatorEnum() != punctuator) {
    SyntaxErrorUnexpectedToken(
        "\'" + Punctuator::ForPunctuatorEnum(punctuator) + "\'");
    ReadToken();
    return;
  }

  ReadToken();
}

bool Parser::StartsProgram(Token &token) {
  return StartsMain(token) || StartsFunctionDefinition(token);
}
std::unique_ptr<ParseNode> Parser::ParseProgram() {
  if (!StartsProgram(*now_reading)) {
    return SyntaxErrorUnexpectedToken("well defined program start");
  }

  std::unique_ptr<ProgramToken> program_token =
      std::make_unique<ProgramToken>();
  std::unique_ptr<ParseNode> program =
      std::make_unique<ProgramNode>(std::move(program_token));

  while (StartsFunctionDefinition(*now_reading)) {
    auto function = ParseFunctionDefinition();
    program->AppendChild(std::move(function));
  }

  std::unique_ptr<ParseNode> main = ParseMain();
  program->AppendChild(std::move(main));

  // if (now_reading->GetTokenType() != TokenType::EOF_TOKEN) {
  //     return SyntaxErrorUnexpectedToken("end of program");
  // }

  return program;
}

bool Parser::StartsFunctionDefinition(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {KeywordEnum::FUNCTION});
}
std::unique_ptr<ParseNode> Parser::ParseFunctionDefinition() {
  if (!StartsFunctionDefinition(*now_reading)) {
    return SyntaxErrorUnexpectedToken("function");
  }

  auto function =
      std::make_unique<FunctionDefinitionNode>(std::move(now_reading));

  // Discard 'function' keyword.
  ReadToken();

  auto function_id = ParseIdentifier();
  if (!function_id) {
    return SyntaxErrorUnexpectedToken("function identifier");
  }
  function->AppendChild(std::move(function_id));

  auto lambda = ParseLambda();
  function->AppendChild(std::move(lambda));

  return function;
}

bool Parser::StartsLambda(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token, {PunctuatorEnum::CMP_LT});
}
std::unique_ptr<ParseNode> Parser::ParseLambda() {
  if (!StartsLambda(*now_reading)) {
    return SyntaxErrorUnexpectedToken("lambda");
  }

  // Discard '<'.
  ReadToken();

  auto lambda_node = std::make_unique<LambdaNode>();

  // Parse parameters.
  if (StartsType(*now_reading)) {
    auto param_type = ParseType();
    auto param_id = ParseIdentifier();
    auto parameter_node = LambdaParameterNode::CreateParameterNode(
        std::move(param_type), std::move(param_id));
    lambda_node->AddParameterNode(std::move(parameter_node));
  }
  while (PunctuatorToken::IsTokenPunctuator(now_reading.get(),
                                            {PunctuatorEnum::SEPARATOR})) {
    // Discard ','.
    ReadToken();
    auto param_type = ParseType();
    auto param_id = ParseIdentifier();
    auto parameter_node = LambdaParameterNode::CreateParameterNode(
        std::move(param_type), std::move(param_id));
    lambda_node->AddParameterNode(std::move(parameter_node));
  }

  // Expect closing '>'.
  Expect(PunctuatorEnum::CMP_GT);

  // Expect '->' before return type.
  Expect(PunctuatorEnum::ARROW);

  // Parse return type.
  std::unique_ptr<ParseNode> return_type = ParseType();
  lambda_node->AddReturnTypeNode(std::move(return_type));

  // Parse lambda body.
  std::unique_ptr<ParseNode> lambda_body = ParseCodeBlock();
  lambda_node->AddLambdaBodyNode(std::move(lambda_body));

  return lambda_node;
}

bool Parser::StartsMain(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {KeywordEnum::MAIN});
}
std::unique_ptr<ParseNode> Parser::ParseMain() {
  if (!StartsMain(*now_reading)) {
    return SyntaxErrorUnexpectedToken("main");
  }

  std::unique_ptr<ParseNode> main =
      std::make_unique<MainNode>(std::move(now_reading));

  ReadToken();

  std::unique_ptr<ParseNode> main_block = ParseCodeBlock();

  main->AppendChild(std::move(main_block));

  return main;
}

bool Parser::StartsStatement(Token &token) {
  return StartsCodeBlock(token) || StartsDeclaration(token) ||
         StartsAssignment(token) || StartsIfStatement(token) ||
         StartsForStatement(token) || StartsCallStatement(token) ||
         StartsPrintStatement(token) || StartsReturnStatement(token);
}
std::unique_ptr<ParseNode> Parser::ParseStatement() {
  if (StartsCodeBlock(*now_reading)) {
    return ParseCodeBlock();
  }
  if (StartsDeclaration(*now_reading)) {
    return ParseDeclaration();
  }
  if (StartsAssignment(*now_reading)) {
    return ParseAssignment();
  }
  if (StartsIfStatement(*now_reading)) {
    return ParseIfStatement();
  }
  if (StartsForStatement(*now_reading)) {
    return ParseForStatement();
  }
  if (StartsCallStatement(*now_reading)) {
    return ParseCallStatement();
  }
  if (StartsPrintStatement(*now_reading)) {
    return ParsePrintStatement();
  }
  if (StartsReturnStatement(*now_reading)) {
    return ParseReturnStatement();
  } else {
    return SyntaxErrorUnexpectedToken("start of statement");
  }
}

bool Parser::StartsCodeBlock(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token,
                                            {PunctuatorEnum::OPEN_BRACE});
}
std::unique_ptr<ParseNode> Parser::ParseCodeBlock() {
  if (!StartsCodeBlock(*now_reading)) {
    return SyntaxErrorUnexpectedToken("code block");
  }

  std::unique_ptr<ParseNode> code_block =
      std::make_unique<CodeBlockNode>(std::move(now_reading));

  ReadToken();

  while (StartsStatement(*now_reading)) {
    std::unique_ptr<ParseNode> statement = ParseStatement();
    code_block->AppendChild(std::move(statement));
  }

  Expect(PunctuatorEnum::CLOSE_BRACE);

  return code_block;
}

bool Parser::StartsDeclaration(Token &token) {
  return KeywordToken::IsTokenKeyword(&token,
                                      {KeywordEnum::CONST, KeywordEnum::VAR});
}
std::unique_ptr<ParseNode> Parser::ParseDeclaration(bool expect_terminator) {
  if (!StartsDeclaration(*now_reading)) {
    return SyntaxErrorUnexpectedToken("declaration statement");
  }

  auto declaration =
      std::make_unique<DeclarationStatementNode>(std::move(now_reading));

  ReadToken();

  auto identifier = ParseIdentifier();

  Expect(PunctuatorEnum::EQUAL);

  auto initializer = ParseExpression();

  if (expect_terminator) {
    Expect(PunctuatorEnum::TERMINATOR);
  }

  declaration->AppendChild(std::move(identifier));
  declaration->AppendChild(std::move(initializer));

  return declaration;
}

bool Parser::StartsAssignment(Token &token) {
  return StartsTargettableExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseAssignment(bool expect_terminator) {
  if (!StartsAssignment(*now_reading)) {
    return SyntaxErrorUnexpectedToken("assignment statement");
  }

  auto assignment = std::make_unique<AssignmentStatementNode>();

  auto target = ParseTargettableExpression();
  assignment->AppendChild(std::move(target));

  Expect(PunctuatorEnum::EQUAL);

  auto new_value = ParseExpression();
  assignment->AppendChild(std::move(new_value));

  if (expect_terminator) {
    Expect(PunctuatorEnum::TERMINATOR);
  }

  return assignment;
}

bool Parser::StartsTargettableExpression(Token &token) {
  return StartsIdentifier(token) || StartsParenthesizedExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseTargettableExpression() {
  if (StartsParenthesizedExpression(*now_reading)) {
    return ParseParenthesizedExpression();
  }
  if (StartsIdentifier(*now_reading)) {
    return ParseIdentifier();
  }
  return SyntaxErrorUnexpectedToken("targettable expression");
}

bool Parser::StartsIfStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {KeywordEnum::IF});
}
bool Parser::StartsElseBlock(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {KeywordEnum::ELSE});
}
std::unique_ptr<ParseNode> Parser::ParseIfStatement() {
  if (!StartsIfStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("if statement");
  }

  auto if_statement = std::make_unique<IfStatementNode>(std::move(now_reading));

  ReadToken();

  auto condition = ParseExpression();

  auto then_block = ParseCodeBlock();

  if_statement->AppendChild(std::move(condition));
  if_statement->AppendChild(std::move(then_block));

  // TODO: We can add `elif` parsing later...
  // while (StartsElifBlock(*now_reading)) {
  //     ...
  // }

  if (StartsElseBlock(*now_reading)) {
    ReadToken();

    auto else_block = ParseCodeBlock();

    if_statement->AppendChild(std::move(else_block));
  }

  return if_statement;
}

bool Parser::StartsForStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {KeywordEnum::FOR});
}
std::unique_ptr<ParseNode> Parser::ParseForStatement() {
  if (!StartsForStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("for statement");
  }

  auto for_statement =
      std::make_unique<ForStatementNode>(std::move(now_reading));

  ReadToken();

  // Append a loop initializer node. If no initializer is specified, a noop is
  // appended.
  std::unique_ptr<ParseNode> init = nullptr;
  if (StartsDeclaration(*now_reading)) {
    init = ParseDeclaration(/* expect_terminator = */ false);
  } else if (StartsAssignment(*now_reading)) {
    init = ParseAssignment(/* expect_terminator = */ false);
  } else {
    init = std::make_unique<NopNode>();
  }
  for_statement->AppendChild(std::move(init));

  Expect(PunctuatorEnum::SEPARATOR);

  // Append a loop condition node. If no condition is specified, condition is
  // always true.
  std::unique_ptr<ParseNode> condition = nullptr;
  if (StartsExpression(*now_reading)) {
    condition = ParseExpression();
  } else {
    condition = std::make_unique<BooleanLiteralNode>(true);
  }
  for_statement->AppendChild(std::move(condition));

  Expect(PunctuatorEnum::SEPARATOR);

  // Append a loop increment node. If no increment is specified, a noop is
  // appended.
  std::unique_ptr<ParseNode> increment = nullptr;
  if (StartsAssignment(*now_reading)) {
    increment = ParseAssignment(/* expect_terminator = */ false);
  } else {
    increment = std::make_unique<NopNode>();
  }
  for_statement->AppendChild(std::move(increment));

  // Append loop body.
  auto loop_body = ParseCodeBlock();
  for_statement->AppendChild(std::move(loop_body));

  return for_statement;
}

bool Parser::StartsCallStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {KeywordEnum::CALL});
}
std::unique_ptr<ParseNode> Parser::ParseCallStatement() {
  if (!StartsCallStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("call statement");
  }

  auto call_statement =
      std::make_unique<CallStatementNode>(std::move(now_reading));

  // Discard 'call' token.
  ReadToken();

  std::unique_ptr<ParseNode> lambda_invocation = nullptr;
  if (StartsIdentifier(*now_reading)) {
    lambda_invocation = ParseIdentifier();
  } else if (StartsLambdaLiteral(*now_reading)) {
    lambda_invocation = ParseLambdaLiteral();
  } else {
    return SyntaxErrorUnexpectedToken("lambda invocation");
  }

  call_statement->AppendChild(std::move(lambda_invocation));

  Expect(PunctuatorEnum::TERMINATOR);

  return call_statement;
}

bool Parser::StartsPrintStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {KeywordEnum::PRINT});
}
std::unique_ptr<ParseNode> Parser::ParsePrintStatement(bool expect_terminator) {
  if (!StartsPrintStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("print statement");
  }

  std::unique_ptr<ParseNode> print_statement =
      std::make_unique<PrintStatementNode>(std::move(now_reading));

  ReadToken();

  print_statement = ParsePrintExpressionList(std::move(print_statement));

  if (expect_terminator) {
    Expect(PunctuatorEnum::TERMINATOR);
  }

  return print_statement;
}

bool Parser::StartsPrintExpressionList(Token &token) {
  return StartsExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParsePrintExpressionList(
    std::unique_ptr<ParseNode> print_statement) {
  if (StartsExpression(*now_reading)) {
    std::unique_ptr<ParseNode> expression = ParseExpression();
    print_statement->AppendChild(std::move(expression));
  } else {
    return SyntaxErrorUnexpectedToken("printable expression");
  }

  while (!PunctuatorToken::IsTokenPunctuator(now_reading.get(),
                                             {PunctuatorEnum::TERMINATOR}) &&
         !now_reading->IsEOF()) {
    Expect(PunctuatorEnum::SEPARATOR);
    std::unique_ptr<ParseNode> expression = ParseExpression();
    print_statement->AppendChild(std::move(expression));
  }

  return print_statement;
}

bool Parser::StartsReturnStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {KeywordEnum::RETURN});
}
std::unique_ptr<ParseNode> Parser::ParseReturnStatement() {
  if (!StartsReturnStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("return statement");
  }

  auto return_statement =
      std::make_unique<ReturnStatementNode>(std::move(now_reading));
  // Discard 'return'.
  ReadToken();

  if (!StartsExpression(*now_reading)) {
    // Void functions do not return a value.
    Expect(PunctuatorEnum::TERMINATOR);
    return return_statement;
  }

  auto return_value = ParseExpression();
  return_statement->AppendChild(std::move(return_value));

  Expect(PunctuatorEnum::TERMINATOR);

  return return_statement;
}

bool Parser::StartsExpression(Token &token) {
  return StartsBooleanExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseExpression() {
  if (!StartsExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("expression");
  }

  return ParseBooleanExpression();
}

bool Parser::StartsBooleanExpression(Token &token) {
  return StartsEqualityExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseBooleanExpression() {
  if (!StartsEqualityExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("boolean expression");
  }

  return ParseEqualityExpression();
}

bool Parser::StartsEqualityExpression(Token &token) {
  return StartsComparisonExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseEqualityExpression() {
  if (!StartsEqualityExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("equality expression");
  }

  auto lhs = ParseComparisonExpression();

  while (PunctuatorToken::IsTokenPunctuator(
      now_reading.get(), {PunctuatorEnum::CMP_EQ, PunctuatorEnum::CMP_NEQ})) {
    auto equality_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    ReadToken();

    auto rhs = ParseComparisonExpression();

    equality_operator->AppendChild(std::move(lhs));
    equality_operator->AppendChild(std::move(rhs));
    lhs = std::move(equality_operator);
  }

  return lhs;
}

bool Parser::StartsComparisonExpression(Token &token) {
  return StartsAdditiveExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseComparisonExpression() {
  if (!StartsComparisonExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("comparison expression");
  }

  auto lhs = ParseAdditiveExpression();

  while (PunctuatorToken::IsTokenPunctuator(
      &(*now_reading), {PunctuatorEnum::CMP_GT, PunctuatorEnum::CMP_LT,
                        PunctuatorEnum::CMP_GEQ, PunctuatorEnum::CMP_LEQ})) {
    auto comparison_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    ReadToken();

    auto rhs = ParseAdditiveExpression();

    comparison_operator->AppendChild(std::move(lhs));
    comparison_operator->AppendChild(std::move(rhs));
    lhs = std::move(comparison_operator);
  }

  return lhs;
}

bool Parser::StartsAdditiveExpression(Token &token) {
  return StartsMultiplicativeExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseAdditiveExpression() {
  if (!StartsAdditiveExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("additive expression");
  }

  std::unique_ptr<ParseNode> left = ParseMultiplicativeExpression();

  while (PunctuatorToken::IsTokenPunctuator(
      now_reading.get(), {PunctuatorEnum::PLUS, PunctuatorEnum::MINUS})) {
    auto additive_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    ReadToken();

    std::unique_ptr<ParseNode> right = ParseMultiplicativeExpression();

    additive_operator->AppendChild(std::move(left));
    additive_operator->AppendChild(std::move(right));
    left = std::move(additive_operator);
  }

  return left;
}

bool Parser::StartsMultiplicativeExpression(Token &token) {
  return StartsUnaryExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseMultiplicativeExpression() {
  if (!StartsMultiplicativeExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("multiplicative expression");
  }

  std::unique_ptr<ParseNode> left = ParseUnaryExpression();

  while (PunctuatorToken::IsTokenPunctuator(
      now_reading.get(), {PunctuatorEnum::MULTIPLY, PunctuatorEnum::DIVIDE})) {
    auto multiplicative_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    ReadToken();

    std::unique_ptr<ParseNode> right = ParseUnaryExpression();

    multiplicative_operator->AppendChild(std::move(left));
    multiplicative_operator->AppendChild(std::move(right));
    left = std::move(multiplicative_operator);
  }

  return left;
}

bool Parser::StartsUnaryExpression(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(
             &token, {PunctuatorEnum::PLUS, PunctuatorEnum::MINUS}) ||
         StartsAtomicExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseUnaryExpression() {
  if (!StartsUnaryExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("unary expression");
  }

  if (PunctuatorToken::IsTokenPunctuator(
          now_reading.get(), {PunctuatorEnum::PLUS, PunctuatorEnum::MINUS})) {
    auto unary_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    ReadToken();

    std::unique_ptr<ParseNode> operand = ParseUnaryExpression();

    unary_operator->AppendChild(std::move(operand));

    return unary_operator;
  } else {
    return ParseAtomicExpression();
  }
}

bool Parser::StartsAtomicExpression(Token &token) {
  return StartsParenthesizedExpression(token) || StartsIdentifier(token) ||
         StartsBooleanLiteral(token) || StartsCharacterLiteral(token) ||
         StartsIntegerLiteral(token) || StartsStringLiteral(token) ||
         StartsLambda(token);
}
std::unique_ptr<ParseNode> Parser::ParseAtomicExpression() {
  if (StartsParenthesizedExpression(*now_reading)) {
    return ParseParenthesizedExpression();
  }
  if (StartsIdentifier(*now_reading)) {
    return ParseIdentifier();
  }
  if (StartsBooleanLiteral(*now_reading)) {
    return ParseBooleanLiteral();
  }
  if (StartsCharacterLiteral(*now_reading)) {
    return ParseCharacterLiteral();
  }
  if (StartsIntegerLiteral(*now_reading)) {
    return ParseIntegerLiteral();
  }
  if (StartsStringLiteral(*now_reading)) {
    return ParseStringLiteral();
  }
  if (StartsLambdaLiteral(*now_reading)) {
    return ParseLambdaLiteral();
  }
  return SyntaxErrorUnexpectedToken("atomic expression");
}

bool Parser::StartsParenthesizedExpression(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token,
                                            {PunctuatorEnum::OPEN_PARENTHESIS});
}
std::unique_ptr<ParseNode> Parser::ParseParenthesizedExpression() {
  if (!StartsParenthesizedExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("parenthesized expression");
  }

  Expect(PunctuatorEnum::OPEN_PARENTHESIS);

  std::unique_ptr<ParseNode> expression = ParseExpression();

  Expect(PunctuatorEnum::CLOSE_PARENTHESIS);

  return expression;
}

bool Parser::StartsIdentifier(Token &token) {
  return token.GetTokenType() == TokenType::IDENTIFIER;
}
std::unique_ptr<ParseNode> Parser::ParseIdentifier() {
  if (!StartsIdentifier(*now_reading)) {
    return SyntaxErrorUnexpectedToken("identifier");
  }

  auto identifier = std::make_unique<IdentifierNode>(std::move(now_reading));

  // Read the identifier.
  ReadToken();

  // If we are not parsing a lambda invocation, return the identifier node.
  if (!StartsLambdaInvocation(*now_reading)) {
    return identifier;
  }
  return ParseLambdaInvocation(std::move(identifier));
}

bool Parser::StartsBooleanLiteral(Token &token) {
  return token.GetTokenType() == TokenType::BOOLEAN_LITERAL;
}
std::unique_ptr<ParseNode> Parser::ParseBooleanLiteral() {
  if (!StartsBooleanLiteral(*now_reading)) {
    return SyntaxErrorUnexpectedToken("boolean literal");
  }

  auto boolean_literal =
      std::make_unique<BooleanLiteralNode>(std::move(now_reading));
  ReadToken();
  return boolean_literal;
}

bool Parser::StartsCharacterLiteral(Token &token) {
  return token.GetTokenType() == TokenType::CHARACTER_LITERAL;
}
std::unique_ptr<ParseNode> Parser::ParseCharacterLiteral() {
  if (!StartsCharacterLiteral(*now_reading)) {
    return SyntaxErrorUnexpectedToken("character literal");
  }

  auto character_literal =
      std::make_unique<CharacterLiteralNode>(std::move(now_reading));
  ReadToken();
  return character_literal;
}

bool Parser::StartsIntegerLiteral(Token &token) {
  return token.GetTokenType() == TokenType::INTEGER_LITERAL;
}
std::unique_ptr<ParseNode> Parser::ParseIntegerLiteral() {
  if (!StartsIntegerLiteral(*now_reading)) {
    return SyntaxErrorUnexpectedToken("integer literal");
  }

  auto integer_literal =
      std::make_unique<IntegerLiteralNode>(std::move(now_reading));
  ReadToken();
  return integer_literal;
}

bool Parser::StartsStringLiteral(Token &token) {
  return token.GetTokenType() == TokenType::STRING_LITERAL;
}
std::unique_ptr<ParseNode> Parser::ParseStringLiteral() {
  if (!StartsStringLiteral(*now_reading)) {
    return SyntaxErrorUnexpectedToken("string literal");
  }

  auto string_literal =
      std::make_unique<StringLiteralNode>(std::move(now_reading));
  ReadToken();
  return string_literal;
}

bool Parser::StartsLambdaLiteral(Token &token) { return StartsLambda(token); }
std::unique_ptr<ParseNode> Parser::ParseLambdaLiteral() {
  if (!StartsLambdaLiteral(*now_reading)) {
    return SyntaxErrorUnexpectedToken("lambda literal");
  }

  auto lambda_literal = ParseLambda();

  // If we are not parsing a lambda invocation, return the lambda literal.
  if (!StartsLambdaInvocation(*now_reading)) {
    return lambda_literal;
  }
  return ParseLambdaInvocation(std::move(lambda_literal));
}

bool Parser::StartsType(Token &token) {
  return StartsBaseType(token) || StartsLambdaType(token);
}
std::unique_ptr<ParseNode> Parser::ParseType() {
  if (StartsBaseType(*now_reading)) {
    return ParseBaseType();
  }
  if (StartsLambdaType(*now_reading)) {
    return ParseLambdaType();
  }
  return SyntaxErrorUnexpectedToken("type");
}

bool Parser::StartsBaseType(Token &token) {
  return KeywordToken::IsTokenKeyword(
      &token, {KeywordEnum::VOID, KeywordEnum::BOOL, KeywordEnum::CHAR,
               KeywordEnum::INT, KeywordEnum::STRING});
}
std::unique_ptr<ParseNode> Parser::ParseBaseType() {
  if (!StartsBaseType(*now_reading)) {
    return SyntaxErrorUnexpectedToken("base type");
  }

  auto type_node = std::make_unique<BaseTypeNode>(std::move(now_reading));
  ReadToken();
  return type_node;
}

bool Parser::StartsLambdaType(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token, {PunctuatorEnum::CMP_LT});
}
std::unique_ptr<ParseNode> Parser::ParseLambdaType() {
  if (!StartsLambdaType(*now_reading)) {
    return SyntaxErrorUnexpectedToken("lambda type");
  }

  auto lambda_type_node =
      std::make_unique<LambdaTypeNode>(std::move(now_reading));
  ReadToken();

  // Parse parameter types.
  if (StartsType(*now_reading)) {
    std::unique_ptr<ParseNode> param_type = ParseType();
    lambda_type_node->AddParameterTypeNode(std::move(param_type));
  }
  while (PunctuatorToken::IsTokenPunctuator(now_reading.get(),
                                            {PunctuatorEnum::SEPARATOR})) {
    // Discard ','.
    ReadToken();
    std::unique_ptr<ParseNode> param_type = ParseType();
    lambda_type_node->AddParameterTypeNode(std::move(param_type));
  }

  // Expect closing '>'.
  Expect(PunctuatorEnum::CMP_GT);

  // Expect '->' before return type.
  Expect(PunctuatorEnum::ARROW);

  // Parse return type.
  std::unique_ptr<ParseNode> return_type = ParseType();
  lambda_type_node->AddReturnTypeNode(std::move(return_type));

  return lambda_type_node;
}

bool Parser::StartsLambdaInvocation(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token,
                                            {PunctuatorEnum::OPEN_PARENTHESIS});
}
std::unique_ptr<ParseNode> Parser::ParseLambdaInvocation(
    std::unique_ptr<ParseNode> lambda) {
  if (!StartsLambdaInvocation(*now_reading)) {
    return SyntaxErrorUnexpectedToken("lambda invocation");
  }

  std::unique_ptr<ParseNode> lambda_invocation = std::move(lambda);
  do {
    lambda = std::move(lambda_invocation);
    Expect(PunctuatorEnum::OPEN_PARENTHESIS);

    std::vector<std::unique_ptr<ParseNode> > args;
    if (StartsExpression(*now_reading)) {
      args.emplace_back(ParseExpression());

      while (PunctuatorToken::IsTokenPunctuator(now_reading.get(),
                                                {PunctuatorEnum::SEPARATOR})) {
        // Discard ','.
        ReadToken();

        args.emplace_back(ParseExpression());
      }
    }

    lambda_invocation =
        LambdaInvocationNode::CreateLambdaInvocationNodeWithArguments(
            std::move(lambda), std::move(args));

    Expect(PunctuatorEnum::CLOSE_PARENTHESIS);
  } while (StartsLambdaInvocation(*now_reading));

  return lambda_invocation;
}

std::unique_ptr<ParseNode> Parser::SyntaxErrorUnexpectedToken(
    std::string expected) {
  std::string message = "Unexpected token \'" + now_reading->GetLexeme() +
                        "\', expected " + expected + " at " +
                        now_reading->GetLocation().ToString();
  PunktLogger::Log(LogType::PARSER, message);
  return GetSyntaxErrorNode();
}

std::unique_ptr<ParseNode> Parser::GetSyntaxErrorNode() {
  auto error = std::make_unique<ErrorNode>(std::move(now_reading));
  ReadToken();
  return error;
}
