#include "parser.h"

#include <logging/punkt_logger.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <scanner/scanner.h>
#include <token/all_tokens.h>

#include <memory>
#include <string>
#include <vector>

std::unique_ptr<ParseNode> Parser::Parse(fs::path file_path) {
  auto scanner = std::make_unique<Scanner>(file_path);
  Parser parser(std::move(scanner));
  std::unique_ptr<ParseNode> ast = parser.ParseProgram();

  auto program_node = dynamic_cast<ProgramNode *>(ast.get());
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

void Parser::Expect(Keyword keyword) {
  auto keyword_token = dynamic_cast<KeywordToken *>(now_reading.get());
  if (!keyword_token || keyword_token->GetKeywordEnum() != keyword) {
    SyntaxErrorUnexpectedToken("\'" + keyword_utils::GetKeywordLexeme(keyword) +
                               "\'");
  }
  ReadToken();
}

void Parser::Expect(Operator op) {
  auto op_token = dynamic_cast<OperatorToken *>(now_reading.get());
  if (!op_token || op_token->GetOperatorEnum() != op) {
    SyntaxErrorUnexpectedToken("\'" + operator_utils::GetOperatorLexeme(op) +
                               "\'");
  }
  ReadToken();
}

void Parser::Expect(Punctuator punc) {
  auto punc_token = dynamic_cast<PunctuatorToken *>(now_reading.get());
  if (!punc_token || punc_token->GetPunctuatorEnum() != punc) {
    SyntaxErrorUnexpectedToken(
        "\'" + punctuator_utils::GetPunctuatorLexeme(punc) + "\'");
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

  auto program_token = std::make_unique<ProgramToken>();
  auto program = std::make_unique<ProgramNode>(std::move(program_token));

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
  return KeywordToken::IsTokenKeyword(&token, {Keyword::FUNCTION});
}
std::unique_ptr<ParseNode> Parser::ParseFunctionDefinition() {
  if (!StartsFunctionDefinition(*now_reading)) {
    return SyntaxErrorUnexpectedToken("function");
  }

  auto function =
      std::make_unique<FunctionDefinitionNode>(std::move(now_reading));

  // Discard 'function' keyword.
  ReadToken();

  std::unique_ptr<ParseNode> function_id = ParseIdentifier();
  if (!function_id) {
    return SyntaxErrorUnexpectedToken("function identifier");
  }
  function->AppendChild(std::move(function_id));

  std::unique_ptr<ParseNode> lambda = ParseLambda();
  function->AppendChild(std::move(lambda));

  return function;
}

bool Parser::StartsLambda(Token &token) {
  return OperatorToken::IsTokenOperator(&token, {Operator::CMP_LT});
}
std::unique_ptr<ParseNode> Parser::ParseLambda() {
  if (!StartsLambda(*now_reading)) {
    return SyntaxErrorUnexpectedToken("lambda");
  }

  auto lambda_node =
      std::make_unique<LambdaNode>(PlaceholderToken::Create(*now_reading));

  // Discard '<'.
  ReadToken();

  // Parse parameters.
  if (StartsType(*now_reading)) {
    auto parameter_node = std::make_unique<LambdaParameterNode>(
        PlaceholderToken::Create(*now_reading));

    std::unique_ptr<ParseNode> param_type = ParseType();

    std::unique_ptr<ParseNode> param_id = ParseIdentifier();

    parameter_node->AppendChild(std::move(param_type));
    parameter_node->AppendChild(std::move(param_id));

    lambda_node->AddParameterNode(std::move(parameter_node));
  }
  while (PunctuatorToken::IsTokenPunctuator(now_reading.get(),
                                            {Punctuator::SEPARATOR})) {
    // Discard ','.
    ReadToken();

    auto parameter_node = std::make_unique<LambdaParameterNode>(
        PlaceholderToken::Create(*now_reading));

    std::unique_ptr<ParseNode> param_type = ParseType();

    std::unique_ptr<ParseNode> param_id = ParseIdentifier();

    parameter_node->AppendChild(std::move(param_type));
    parameter_node->AppendChild(std::move(param_id));

    lambda_node->AddParameterNode(std::move(parameter_node));
  }

  // Expect closing '>'.
  Expect(Operator::CMP_GT);

  // Expect '->' before return type.
  Expect(Punctuator::ARROW);

  // Parse return type.
  std::unique_ptr<ParseNode> return_type = ParseType();
  lambda_node->AddReturnTypeNode(std::move(return_type));

  // Parse lambda body.
  std::unique_ptr<ParseNode> lambda_body = ParseCodeBlock();
  lambda_node->AddLambdaBodyNode(std::move(lambda_body));

  return lambda_node;
}

bool Parser::StartsMain(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::MAIN});
}
std::unique_ptr<ParseNode> Parser::ParseMain() {
  if (!StartsMain(*now_reading)) {
    return SyntaxErrorUnexpectedToken("main");
  }

  auto main = std::make_unique<MainNode>(std::move(now_reading));

  ReadToken();

  std::unique_ptr<ParseNode> main_block = ParseCodeBlock();

  main->AppendChild(std::move(main_block));

  return main;
}

bool Parser::StartsStatement(Token &token) {
  return StartsCodeBlock(token) || StartsDeclaration(token) ||
         StartsExpressionStatement(token) || StartsIfStatement(token) ||
         StartsWhileStatement(token) || StartsForStatement(token) ||
         StartsDeallocStatement(token) || StartsCallStatement(token) ||
         StartsPrintStatement(token) || StartsReturnStatement(token);
}
std::unique_ptr<ParseNode> Parser::ParseStatement() {
  if (StartsCodeBlock(*now_reading)) {
    return ParseCodeBlock();
  }
  if (StartsDeclaration(*now_reading)) {
    return ParseDeclaration();
  }
  if (StartsExpressionStatement(*now_reading)) {
    return ParseExpressionStatement();
  }
  if (StartsIfStatement(*now_reading)) {
    return ParseIfStatement();
  }
  if (StartsWhileStatement(*now_reading)) {
    return ParseWhileStatement();
  }
  if (StartsForStatement(*now_reading)) {
    return ParseForStatement();
  }
  if (StartsDeallocStatement(*now_reading)) {
    return ParseDeallocStatement();
  }
  if (StartsCallStatement(*now_reading)) {
    return ParseCallStatement();
  }
  if (StartsReturnStatement(*now_reading)) {
    return ParseReturnStatement();
  }
  if (StartsPrintStatement(*now_reading)) {
    return ParsePrintStatement();
  }

  return SyntaxErrorUnexpectedToken("start of statement");
}

bool Parser::StartsCodeBlock(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token, {Punctuator::OPEN_BRACE});
}
std::unique_ptr<ParseNode> Parser::ParseCodeBlock() {
  if (!StartsCodeBlock(*now_reading)) {
    return SyntaxErrorUnexpectedToken("code block");
  }

  auto code_block = std::make_unique<CodeBlockNode>(std::move(now_reading));

  ReadToken();

  while (StartsStatement(*now_reading)) {
    std::unique_ptr<ParseNode> statement = ParseStatement();
    code_block->AppendChild(std::move(statement));
  }

  Expect(Punctuator::CLOSE_BRACE);

  return code_block;
}

bool Parser::StartsDeclaration(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::CONST, Keyword::VAR});
}
std::unique_ptr<ParseNode> Parser::ParseDeclaration(bool expect_terminator) {
  if (!StartsDeclaration(*now_reading)) {
    return SyntaxErrorUnexpectedToken("declaration statement");
  }

  auto declaration =
      std::make_unique<DeclarationStatementNode>(std::move(now_reading));

  ReadToken();

  std::unique_ptr<ParseNode> identifier = ParseIdentifier();

  Expect(Operator::ASSIGN);

  std::unique_ptr<ParseNode> initializer = ParseExpression();

  if (expect_terminator) {
    Expect(Punctuator::TERMINATOR);
  }

  declaration->AppendChild(std::move(identifier));
  declaration->AppendChild(std::move(initializer));

  return declaration;
}

bool Parser::StartsExpressionStatement(Token &token) {
  return StartsExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseExpressionStatement(
    bool expect_terminator) {
  if (!StartsExpressionStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("expression statement");
  }

  auto expr_stmt_node = std::make_unique<ExpressionStatementNode>(
      PlaceholderToken::Create(*now_reading));

  std::unique_ptr<ParseNode> expr = ParseExpression();

  expr_stmt_node->AppendChild(std::move(expr));

  if (expect_terminator) {
    Expect(Punctuator::TERMINATOR);
  }

  return expr_stmt_node;
}

bool Parser::StartsIfStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::IF});
}
bool Parser::StartsElseBlock(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::ELSE});
}
std::unique_ptr<ParseNode> Parser::ParseIfStatement() {
  if (!StartsIfStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("if statement");
  }

  auto if_statement = std::make_unique<IfStatementNode>(std::move(now_reading));

  // Discard 'if'.
  ReadToken();

  std::unique_ptr<ParseNode> if_condition = ParseExpression();
  std::unique_ptr<ParseNode> if_block = nullptr;

  if (StartsCodeBlock(*now_reading)) {
    if_block = ParseCodeBlock();
  } else {
    if_block = ParseStatement();
  }

  if_statement->AppendIf(std::move(if_condition), std::move(if_block));

  if (StartsElseBlock(*now_reading)) {
    // Discard 'else'.
    ReadToken();

    std::unique_ptr<ParseNode> else_block = nullptr;
    if (StartsCodeBlock(*now_reading)) {
      else_block = ParseCodeBlock();
    } else {
      else_block = ParseStatement();
    }

    if_statement->AppendElse(std::move(else_block));
  }

  return if_statement;
}

bool Parser::StartsWhileStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::WHILE});
}
std::unique_ptr<ParseNode> Parser::ParseWhileStatement() {
  if (!StartsWhileStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("while statement");
  }

  auto while_stmt =
      std::make_unique<WhileStatementNode>(std::move(now_reading));

  // Discard 'while'.
  ReadToken();

  std::unique_ptr<ParseNode> condition = ParseExpression();
  std::unique_ptr<ParseNode> while_block = nullptr;

  if (StartsCodeBlock(*now_reading)) {
    while_block = ParseCodeBlock();
  } else {
    while_block = ParseStatement();
  }

  while_stmt->AppendChild(std::move(condition));
  while_stmt->AppendChild(std::move(while_block));

  return while_stmt;
}

bool Parser::StartsForStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::FOR});
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
  } else if (StartsAssignmentExpression(*now_reading)) {
    init = ParseAssignmentExpression();
  } else {
    init = std::make_unique<NopNode>(PlaceholderToken::Create(*now_reading));
  }
  for_statement->AppendChild(std::move(init));

  Expect(Punctuator::SEPARATOR);

  // Append a loop condition node. If no condition is specified, condition is
  // always true.
  std::unique_ptr<ParseNode> condition = nullptr;
  if (StartsExpression(*now_reading)) {
    condition = ParseExpression();
  } else {
    auto cond_token = std::make_unique<BooleanLiteralToken>(
        "true", now_reading->GetLocation(), true);
    condition = std::make_unique<BooleanLiteralNode>(std::move(cond_token));
  }
  for_statement->AppendChild(std::move(condition));

  Expect(Punctuator::SEPARATOR);

  // Append a loop increment node. If no increment is specified, a noop is
  // appended.
  std::unique_ptr<ParseNode> increment = nullptr;
  if (StartsAssignmentExpression(*now_reading)) {
    increment = ParseAssignmentExpression();
  } else {
    increment =
        std::make_unique<NopNode>(PlaceholderToken::Create(*now_reading));
  }
  for_statement->AppendChild(std::move(increment));

  // Append loop body.
  std::unique_ptr<ParseNode> loop_body = ParseCodeBlock();
  for_statement->AppendChild(std::move(loop_body));

  return for_statement;
}

bool Parser::StartsDeallocStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::DEALLOC});
}
std::unique_ptr<ParseNode> Parser::ParseDeallocStatement() {
  if (!StartsDeallocStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("dealloc statement");
  }

  auto dealloc_stmt =
      std::make_unique<DeallocStatementNode>(std::move(now_reading));

  // Discard 'dealloc' token.
  ReadToken();

  // Parse argument.
  std::unique_ptr<ParseNode> arg = ParseExpression();
  dealloc_stmt->AppendChild(std::move(arg));

  Expect(Punctuator::TERMINATOR);

  return dealloc_stmt;
}

bool Parser::StartsCallStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::CALL});
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
    lambda_invocation = ParseIdentifierAtomic();
  } else if (StartsLambdaLiteral(*now_reading)) {
    lambda_invocation = ParseLambdaLiteral();
  } else {
    return SyntaxErrorUnexpectedToken("lambda invocation");
  }

  call_statement->AppendChild(std::move(lambda_invocation));

  Expect(Punctuator::TERMINATOR);

  return call_statement;
}

bool Parser::StartsPrintStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token,
                                      {Keyword::PRINT, Keyword::PRINTLN});
}
std::unique_ptr<ParseNode> Parser::ParsePrintStatement() {
  if (!StartsPrintStatement(*now_reading)) {
    return SyntaxErrorUnexpectedToken("print statement");
  }

  // The `println` keyword means that we should print a line feed char at the
  // end.
  bool is_println =
      KeywordToken::IsTokenKeyword(now_reading.get(), {Keyword::PRINTLN});

  auto print_statement =
      std::make_unique<PrintStatementNode>(std::move(now_reading), is_println);

  // Discard 'print' or 'println'.
  ReadToken();

  // Parse expressions to be printed.
  std::unique_ptr<ParseNode> print_expr = ParseExpression();
  print_statement->AppendChild(std::move(print_expr));

  while (PunctuatorToken::IsTokenPunctuator(now_reading.get(),
                                            {Punctuator::SEPARATOR})) {
    // Discard ','.
    ReadToken();

    std::unique_ptr<ParseNode> print_expr = ParseExpression();
    print_statement->AppendChild(std::move(print_expr));
  }

  Expect(Punctuator::TERMINATOR);

  return print_statement;
}

bool Parser::StartsReturnStatement(Token &token) {
  return KeywordToken::IsTokenKeyword(&token, {Keyword::RETURN});
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
    Expect(Punctuator::TERMINATOR);
    return return_statement;
  }

  std::unique_ptr<ParseNode> return_value = ParseExpression();
  return_statement->AppendChild(std::move(return_value));

  Expect(Punctuator::TERMINATOR);

  return return_statement;
}

bool Parser::StartsExpression(Token &token) {
  return StartsAssignmentExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseExpression() {
  if (!StartsExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("expression");
  }

  return ParseAssignmentExpression();
}

bool Parser::StartsAssignmentExpression(Token &token) {
  return StartsBooleanORExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseAssignmentExpression() {
  if (!StartsAssignmentExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("assignment expression");
  }

  std::unique_ptr<ParseNode> target = ParseBooleanORExpression();

  // Parsing regular assignment (=).
  if (OperatorToken::IsTokenOperator(now_reading.get(), {Operator::ASSIGN})) {
    auto assign_op = std::make_unique<OperatorNode>(std::move(now_reading));
    ReadToken();
    std::unique_ptr<ParseNode> new_value = ParseBooleanORExpression();

    assign_op->AppendChild(std::move(target));
    assign_op->AppendChild(std::move(new_value));
    return assign_op;
  }

  // Parsing combined assignment (+=, -=, etc).
  if (OperatorToken::IsTokenOperator(
          now_reading.get(),
          {Operator::ADD_ASSIGN, Operator::SUB_ASSIGN, Operator::MUL_ASSIGN,
           Operator::DIV_ASSIGN, Operator::MOD_ASSIGN})) {
    // Create '=' token.
    auto assign_token = std::make_unique<OperatorToken>(
        "=", now_reading->GetLocation(), Operator::ASSIGN);

    // Create the operator token.
    std::unique_ptr<OperatorToken> op_token = nullptr;
    OperatorToken *now_reading_operator =
        static_cast<OperatorToken *>(now_reading.get());
    switch (now_reading_operator->GetOperatorEnum()) {
      case Operator::ADD_ASSIGN:
        op_token = std::make_unique<OperatorToken>(
            "+", now_reading->GetLocation(), Operator::PLUS);
        break;
      case Operator::SUB_ASSIGN:
        op_token = std::make_unique<OperatorToken>(
            "-", now_reading->GetLocation(), Operator::MINUS);
        break;
      case Operator::MUL_ASSIGN:
        op_token = std::make_unique<OperatorToken>(
            "*", now_reading->GetLocation(), Operator::MUL);
        break;
      case Operator::DIV_ASSIGN:
        op_token = std::make_unique<OperatorToken>(
            "/", now_reading->GetLocation(), Operator::DIV);
        break;
      case Operator::MOD_ASSIGN:
        op_token = std::make_unique<OperatorToken>(
            "%", now_reading->GetLocation(), Operator::MOD);
        break;
      default:
        PunktLogger::LogFatalInternalError("unexpected assignment operator");
    }

    // Discard the combined assignment token.
    ReadToken();

    // Create a copy of the target node here.
    std::unique_ptr<ParseNode> lhs = target->CreateCopy();

    // Create assignment operator node.
    auto assign_op = std::make_unique<OperatorNode>(std::move(assign_token));
    // Place the target node on the left of the assignment operator.
    assign_op->AppendChild(std::move(target));

    // Create addition operator node.
    auto op_node = std::make_unique<OperatorNode>(std::move(op_token));
    // Append the left-hand side operand.
    op_node->AppendChild(std::move(lhs));
    // Parse the right-hand side of the operand.
    std::unique_ptr<ParseNode> rhs = ParseBooleanORExpression();
    // Append the right-hand side operand.
    op_node->AppendChild(std::move(rhs));
    // Append operator to assignment.
    assign_op->AppendChild(std::move(op_node));

    return assign_op;
  }

  return target;
}

bool Parser::StartsBooleanORExpression(Token &token) {
  return StartsBooleanANDExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseBooleanORExpression() {
  if (!StartsBooleanORExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("boolean OR expression");
  }

  std::unique_ptr<ParseNode> lhs = ParseBooleanANDExpression();

  while (
      OperatorToken::IsTokenOperator(now_reading.get(), {Operator::BOOL_OR})) {
    auto bool_or_op = std::make_unique<OperatorNode>(std::move(now_reading));

    // Discard '||'.
    ReadToken();

    std::unique_ptr<ParseNode> rhs = ParseBooleanANDExpression();

    bool_or_op->AppendChild(std::move(lhs));
    bool_or_op->AppendChild(std::move(rhs));
    lhs = std::move(bool_or_op);
  }

  return lhs;
}

bool Parser::StartsBooleanANDExpression(Token &token) {
  return StartsEqualityExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseBooleanANDExpression() {
  if (!StartsBooleanANDExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("boolean AND expression");
  }

  std::unique_ptr<ParseNode> lhs = ParseEqualityExpression();

  while (
      OperatorToken::IsTokenOperator(now_reading.get(), {Operator::BOOL_AND})) {
    auto bool_and_op = std::make_unique<OperatorNode>(std::move(now_reading));

    // Discard '&&'.
    ReadToken();

    std::unique_ptr<ParseNode> rhs = ParseEqualityExpression();

    bool_and_op->AppendChild(std::move(lhs));
    bool_and_op->AppendChild(std::move(rhs));
    lhs = std::move(bool_and_op);
  }

  return lhs;
}

bool Parser::StartsEqualityExpression(Token &token) {
  return StartsComparisonExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseEqualityExpression() {
  if (!StartsEqualityExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("equality expression");
  }

  std::unique_ptr<ParseNode> lhs = ParseComparisonExpression();

  while (OperatorToken::IsTokenOperator(
      now_reading.get(), {Operator::CMP_EQ, Operator::CMP_NEQ})) {
    auto equality_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    ReadToken();

    std::unique_ptr<ParseNode> rhs = ParseComparisonExpression();

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

  std::unique_ptr<ParseNode> lhs = ParseAdditiveExpression();

  while (OperatorToken::IsTokenOperator(
      now_reading.get(), {Operator::CMP_GT, Operator::CMP_LT, Operator::CMP_GEQ,
                          Operator::CMP_LEQ})) {
    auto comparison_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    ReadToken();

    std::unique_ptr<ParseNode> rhs = ParseAdditiveExpression();

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

  while (OperatorToken::IsTokenOperator(now_reading.get(),
                                        {Operator::PLUS, Operator::MINUS})) {
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

  while (OperatorToken::IsTokenOperator(
      now_reading.get(), {Operator::MUL, Operator::DIV, Operator::MOD})) {
    auto multiplicative_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    // Discard operator token.
    ReadToken();

    std::unique_ptr<ParseNode> right = ParseUnaryExpression();

    multiplicative_operator->AppendChild(std::move(left));
    multiplicative_operator->AppendChild(std::move(right));
    left = std::move(multiplicative_operator);
  }

  return left;
}

bool Parser::StartsUnaryExpression(Token &token) {
  return OperatorToken::IsTokenOperator(
             &token, {Operator::BOOL_NOT, Operator::PLUS, Operator::MINUS,
                      Operator::SIZEOF}) ||
         StartsAtomicExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseUnaryExpression() {
  if (!StartsUnaryExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("unary expression");
  }

  if (OperatorToken::IsTokenOperator(now_reading.get(),
                                     {Operator::BOOL_NOT, Operator::PLUS,
                                      Operator::MINUS, Operator::SIZEOF})) {
    auto unary_operator =
        std::make_unique<OperatorNode>(std::move(now_reading));

    // Discard operator token.
    ReadToken();

    std::unique_ptr<ParseNode> operand = ParseUnaryExpression();

    unary_operator->AppendChild(std::move(operand));

    return unary_operator;
  } else {
    return ParseAtomicExpression();
  }
}

bool Parser::StartsAtomicExpression(Token &token) {
  return StartsParenthesizedExpression(token) ||
         StartsIdentifierAtomic(token) || StartsBooleanLiteral(token) ||
         StartsCharacterLiteral(token) || StartsIntegerLiteral(token) ||
         StartsStringLiteral(token) || StartsAllocExpression(token) ||
         StartsLambda(token);
}
std::unique_ptr<ParseNode> Parser::ParseAtomicExpression() {
  if (StartsParenthesizedExpression(*now_reading)) {
    return ParseParenthesizedExpression();
  }
  if (StartsIdentifierAtomic(*now_reading)) {
    return ParseIdentifierAtomic();
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
  if (StartsAllocExpression(*now_reading)) {
    return ParseAllocExpression();
  }
  if (StartsLambdaLiteral(*now_reading)) {
    return ParseLambdaLiteral();
  }
  return SyntaxErrorUnexpectedToken("atomic expression");
}

bool Parser::StartsParenthesizedExpression(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token,
                                            {Punctuator::OPEN_PARENTHESIS});
}
std::unique_ptr<ParseNode> Parser::ParseParenthesizedExpression() {
  if (!StartsParenthesizedExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("parenthesized expression");
  }

  Expect(Punctuator::OPEN_PARENTHESIS);

  std::unique_ptr<ParseNode> expression = ParseExpression();

  Expect(Punctuator::CLOSE_PARENTHESIS);

  return expression;
}

bool Parser::StartsIdentifierAtomic(Token &token) {
  return StartsIdentifier(token);
}
std::unique_ptr<ParseNode> Parser::ParseIdentifierAtomic() {
  if (!StartsIdentifierAtomic(*now_reading)) {
    return SyntaxErrorUnexpectedToken("identifier atomic");
  }

  std::unique_ptr<ParseNode> id_atomic = ParseIdentifier();

  while (StartsLambdaInvocation(*now_reading) ||
         StartsArrayIndexing(*now_reading)) {
    if (StartsArrayIndexing(*now_reading)) {
      id_atomic = ParseArrayIndexing(std::move(id_atomic));
    } else {
      id_atomic = ParseLambdaInvocation(std::move(id_atomic));
    }
  }

  return id_atomic;
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

  return identifier;
}

bool Parser::StartsArrayIndexing(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token, {Punctuator::OPEN_BRACKET});
}
std::unique_ptr<ParseNode> Parser::ParseArrayIndexing(
    std::unique_ptr<ParseNode> arr) {
  if (!StartsArrayIndexing(*now_reading)) {
    return SyntaxErrorUnexpectedToken("array indexing");
  }

  auto arr_idx_token = std::make_unique<OperatorToken>(
      "[]", now_reading->GetLocation(), Operator::ARRAY_IDX);

  // Discard '['.
  ReadToken();

  std::unique_ptr<ParseNode> expr = ParseExpression();

  // Initialize indexing operator node.
  auto idx_op_node = std::make_unique<OperatorNode>(std::move(arr_idx_token));
  idx_op_node->AppendChild(std::move(arr));
  idx_op_node->AppendChild(std::move(expr));

  // Expect closing ']'.
  Expect(Punctuator::CLOSE_BRACKET);

  return idx_op_node;
}

bool Parser::StartsLambdaInvocation(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token,
                                            {Punctuator::OPEN_PARENTHESIS});
}
std::unique_ptr<ParseNode> Parser::ParseLambdaInvocation(
    std::unique_ptr<ParseNode> lambda) {
  if (!StartsLambdaInvocation(*now_reading)) {
    return SyntaxErrorUnexpectedToken("lambda invocation");
  }

  // Discard '('.
  ReadToken();

  std::vector<std::unique_ptr<ParseNode>> args;
  if (StartsExpression(*now_reading)) {
    args.push_back(ParseExpression());

    while (PunctuatorToken::IsTokenPunctuator(now_reading.get(),
                                              {Punctuator::SEPARATOR})) {
      // Discard ','.
      ReadToken();
      args.push_back(ParseExpression());
    }
  }

  // Initialize lambda invocation node.
  auto lambda_inv = std::make_unique<LambdaInvocationNode>(
      PlaceholderToken::Create(*now_reading));
  // Append lambda that is to be invoked.
  lambda_inv->AppendChild(std::move(lambda));
  // Append lambda arguments.
  for (unsigned i = 0, n = args.size(); i < n; ++i) {
    lambda_inv->AppendChild(std::move(args.at(i)));
  }

  Expect(Punctuator::CLOSE_PARENTHESIS);

  return lambda_inv;
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

bool Parser::StartsAllocExpression(Token &token) {
  return OperatorToken::IsTokenOperator(&token, {Operator::ALLOC});
}
std::unique_ptr<ParseNode> Parser::ParseAllocExpression() {
  if (!StartsAllocExpression(*now_reading)) {
    return SyntaxErrorUnexpectedToken("alloc statement");
  }

  std::unique_ptr<ParseNode> alloc_expr =
      std::make_unique<OperatorNode>(std::move(now_reading));

  // Discard 'alloc' token.
  ReadToken();

  // Parse array type.
  std::unique_ptr<ParseNode> array_type = ParseArrayType();
  alloc_expr->AppendChild(std::move(array_type));

  // Parse array size.
  Expect(Punctuator::OPEN_PARENTHESIS);
  std::unique_ptr<ParseNode> expr = ParseExpression();
  alloc_expr->AppendChild(std::move(expr));
  Expect(Punctuator::CLOSE_PARENTHESIS);

  while (StartsLambdaInvocation(*now_reading) ||
         StartsArrayIndexing(*now_reading)) {
    if (StartsArrayIndexing(*now_reading)) {
      alloc_expr = ParseArrayIndexing(std::move(alloc_expr));
    } else {
      alloc_expr = ParseLambdaInvocation(std::move(alloc_expr));
    }
  }

  return alloc_expr;
}

bool Parser::StartsLambdaLiteral(Token &token) { return StartsLambda(token); }
std::unique_ptr<ParseNode> Parser::ParseLambdaLiteral() {
  if (!StartsLambdaLiteral(*now_reading)) {
    return SyntaxErrorUnexpectedToken("lambda literal");
  }

  std::unique_ptr<ParseNode> lambda_literal = ParseLambda();

  while (StartsLambdaInvocation(*now_reading) ||
         StartsArrayIndexing(*now_reading)) {
    if (StartsArrayIndexing(*now_reading)) {
      lambda_literal = ParseArrayIndexing(std::move(lambda_literal));
    } else {
      lambda_literal = ParseLambdaInvocation(std::move(lambda_literal));
    }
  }

  return lambda_literal;
}

bool Parser::StartsType(Token &token) {
  return StartsBaseType(token) || StartsArrayType(token) ||
         StartsLambdaType(token);
}
std::unique_ptr<ParseNode> Parser::ParseType() {
  if (StartsBaseType(*now_reading)) {
    return ParseBaseType();
  }
  if (StartsArrayType(*now_reading)) {
    return ParseArrayType();
  }
  if (StartsLambdaType(*now_reading)) {
    return ParseLambdaType();
  }
  return SyntaxErrorUnexpectedToken("type");
}

bool Parser::StartsBaseType(Token &token) {
  return KeywordToken::IsTokenKeyword(
      &token, {Keyword::VOID, Keyword::BOOL, Keyword::CHAR, Keyword::INT,
               Keyword::STRING});
}
std::unique_ptr<ParseNode> Parser::ParseBaseType() {
  if (!StartsBaseType(*now_reading)) {
    return SyntaxErrorUnexpectedToken("base type");
  }

  auto type_node = std::make_unique<BaseTypeNode>(std::move(now_reading));
  ReadToken();
  return type_node;
}

bool Parser::StartsArrayType(Token &token) {
  return PunctuatorToken::IsTokenPunctuator(&token, {Punctuator::OPEN_BRACKET});
}
std::unique_ptr<ParseNode> Parser::ParseArrayType() {
  if (!StartsArrayType(*now_reading)) {
    return SyntaxErrorUnexpectedToken("array type");
  }

  auto arr_type_node =
      std::make_unique<ArrayTypeNode>(PlaceholderToken::Create(*now_reading));

  // Discard '['.
  ReadToken();

  // Parse array subtype.
  std::unique_ptr<ParseNode> subtype = ParseType();
  arr_type_node->AppendChild(std::move(subtype));

  // Expect closing ']'.
  Expect(Punctuator::CLOSE_BRACKET);

  return arr_type_node;
}

bool Parser::StartsLambdaType(Token &token) {
  return OperatorToken::IsTokenOperator(&token, {Operator::CMP_LT});
}
std::unique_ptr<ParseNode> Parser::ParseLambdaType() {
  if (!StartsLambdaType(*now_reading)) {
    return SyntaxErrorUnexpectedToken("lambda type");
  }

  auto lambda_type_node =
      std::make_unique<LambdaTypeNode>(PlaceholderToken::Create(*now_reading));
  ReadToken();

  // Parse parameter types.
  if (StartsType(*now_reading)) {
    std::unique_ptr<ParseNode> param_type = ParseType();
    lambda_type_node->AddParameterTypeNode(std::move(param_type));
  }
  while (PunctuatorToken::IsTokenPunctuator(now_reading.get(),
                                            {Punctuator::SEPARATOR})) {
    // Discard ','.
    ReadToken();
    std::unique_ptr<ParseNode> param_type = ParseType();
    lambda_type_node->AddParameterTypeNode(std::move(param_type));
  }

  // Expect closing '>'.
  Expect(Operator::CMP_GT);

  // Expect '->' before return type.
  Expect(Punctuator::ARROW);

  // Parse return type.
  std::unique_ptr<ParseNode> return_type = ParseType();
  lambda_type_node->AddReturnTypeNode(std::move(return_type));

  return lambda_type_node;
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
