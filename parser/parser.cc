#include <experimental/memory>

#include <token/all_tokens.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <logging/punkt_logger.h>

#include "parser.h"

std::unique_ptr<ParseNode> Parser::Parse(fs::path file_path) {
    std::unique_ptr<Scanner> scanner = std::make_unique<Scanner>(file_path);
    Parser parser(std::move(scanner));
    auto ast = parser.ParseProgram();
    
    // TODO: This cast causes an exception on bad input.
    // Example:
    // main { else {} }
    ProgramNode& program_node = dynamic_cast<ProgramNode&>(*ast);
    program_node.SetModuleID(file_path.string());

    return ast;
}

Parser::Parser(std::unique_ptr<Scanner> scanner) : scanner(std::move(scanner)) {
    ReadToken();
}

void Parser::ReadToken() {
    now_reading = scanner->Next();
    if (!now_reading) {
        PunktLogger::LogFatalInternalError("failed to read token in Parser::ReadToken");
    }
}

void Parser::Expect(KeywordEnum keyword) {
    if (now_reading->GetTokenType() != TokenType::KEYWORD) {
        SyntaxErrorUnexpectedToken("\'" + Keyword::ForKeywordEnum(keyword) + "\'");
        ReadToken();
        return;
    }

    KeywordToken& keyword_token = dynamic_cast<KeywordToken&>(*now_reading);
    if (keyword_token.GetKeywordEnum() != keyword) {
        SyntaxErrorUnexpectedToken("\'" + Keyword::ForKeywordEnum(keyword) + "\'");
        ReadToken();
        return;
    }

    ReadToken();
}

void Parser::Expect(PunctuatorEnum punctuator) {
    if (now_reading->GetTokenType() != TokenType::PUNCTUATOR) {
        SyntaxErrorUnexpectedToken("\'" + Punctuator::ForPunctuatorEnum(punctuator) + "\'");
        ReadToken();
        return;
    }

    PunctuatorToken& punctuator_token = dynamic_cast<PunctuatorToken&>(*now_reading);
    if (punctuator_token.GetPunctuatorEnum() != punctuator) {
        SyntaxErrorUnexpectedToken("\'" + Punctuator::ForPunctuatorEnum(punctuator) + "\'");
        ReadToken();
        return;
    }

    ReadToken();
}

bool Parser::StartsProgram(Token& token) {
    return StartsMain(token)
        || StartsFunction(token);
}
std::unique_ptr<ParseNode> Parser::ParseProgram() {
    if (!StartsProgram(*now_reading)) {
        return SyntaxErrorUnexpectedToken("well defined program start");
    }

    std::unique_ptr<ProgramToken> program_token = std::make_unique<ProgramToken>();
    std::unique_ptr<ParseNode> program = std::make_unique<ProgramNode>(std::move(program_token));

    while (StartsFunction(*now_reading)) {
        auto function = ParseFunction();
        program->AppendChild(std::move(function));
    }

    std::unique_ptr<ParseNode> main = ParseMain();
    program->AppendChild(std::move(main));

    // if (now_reading->GetTokenType() != TokenType::EOF_TOKEN) {
    //     return SyntaxErrorUnexpectedToken("end of program");
    // }

    return program;
}

bool Parser::StartsFunction(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {KeywordEnum::FUNCTION});
}
std::unique_ptr<ParseNode> Parser::ParseFunction() {
    if (!StartsFunction(*now_reading)) {
        return SyntaxErrorUnexpectedToken("function");
    }

    auto function = std::make_unique<FunctionNode>(std::move(now_reading));

    // Discard 'function' keyword.
    ReadToken();

    auto function_id = ParseIdentifier();
    if (!function_id) {
        return SyntaxErrorUnexpectedToken("function identifier");
    }
    function->AppendChild(std::move(function_id));

    auto function_prototype = ParseFunctionPrototype();
    function->AppendChild(std::move(function_prototype));

    auto function_body = ParseCodeBlock();
    function->AppendChild(std::move(function_body));

    return function;
}

bool Parser::StartsFunctionPrototype(Token& token) {
    return PunctuatorToken::IsTokenPunctuator(token, {PunctuatorEnum::OPEN_PARENTHESIS});
}
std::unique_ptr<ParseNode> Parser::ParseFunctionPrototype() {
    if (!StartsFunctionPrototype(*now_reading)) {
        return SyntaxErrorUnexpectedToken("function prototype");
    }

    // Discard '('.
    ReadToken();

    auto prototype = std::make_unique<FunctionPrototypeNode>();

    // Parse parameters.
    if (StartsType(*now_reading)) {
        auto param_type = ParseType();
        auto param_id = ParseIdentifier();
        auto parameter_node = FunctionParameterNode::CreateParameterNode(std::move(param_type),
                std::move(param_id));
        prototype->AppendChild(std::move(parameter_node));
    }
    while (PunctuatorToken::IsTokenPunctuator(*now_reading, {PunctuatorEnum::SEPARATOR})) {
        // Discard ','.
        ReadToken();
        auto param_type = ParseType();
        auto param_id = ParseIdentifier();
        auto parameter_node = FunctionParameterNode::CreateParameterNode(std::move(param_type),
                std::move(param_id));
        prototype->AppendChild(std::move(parameter_node));
    }

    Expect(PunctuatorEnum::CLOSE_PARENTHESIS);

    Expect(PunctuatorEnum::ARROW);

    // Parse return type.
    auto return_type = ParseType();
    prototype->AppendChild(std::move(return_type));

    return prototype;
}

bool Parser::StartsMain(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {KeywordEnum::MAIN});
}
std::unique_ptr<ParseNode> Parser::ParseMain() {
    if (!StartsMain(*now_reading)) {
        return SyntaxErrorUnexpectedToken("main");
    }

    std::unique_ptr<ParseNode> main = std::make_unique<MainNode>(std::move(now_reading));

    ReadToken();

    std::unique_ptr<ParseNode> main_block = ParseCodeBlock();

    main->AppendChild(std::move(main_block));

    return main;
}

bool Parser::StartsStatement(Token& token) {
    return StartsCodeBlock(token)
        || StartsDeclaration(token)
        || StartsAssignment(token)
        || StartsIfStatement(token)
        || StartsForStatement(token)
        || StartsPrintStatement(token)
        || StartsReturnStatement(token);
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
    if (StartsPrintStatement(*now_reading)) {
        return ParsePrintStatement();
    }
    if (StartsReturnStatement(*now_reading)) {
        return ParseReturnStatement();
    }
    else {
        return SyntaxErrorUnexpectedToken("start of statement");
    }
}

bool Parser::StartsCodeBlock(Token& token) {
    return PunctuatorToken::IsTokenPunctuator(token, {PunctuatorEnum::OPEN_BRACE});
}
std::unique_ptr<ParseNode> Parser::ParseCodeBlock() {
    if (!StartsCodeBlock(*now_reading)) {
        return SyntaxErrorUnexpectedToken("code block");
    }

    std::unique_ptr<ParseNode> code_block = std::make_unique<CodeBlockNode>(std::move(now_reading));

    ReadToken();

    while (StartsStatement(*now_reading)) {
        std::unique_ptr<ParseNode> statement = ParseStatement();
        code_block->AppendChild(std::move(statement));
    }

    Expect(PunctuatorEnum::CLOSE_BRACE);

    return code_block;
}

bool Parser::StartsDeclaration(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {KeywordEnum::CONST, KeywordEnum::VAR});
}
std::unique_ptr<ParseNode> Parser::ParseDeclaration(bool expect_terminator) {
    if (!StartsDeclaration(*now_reading)) {
        return SyntaxErrorUnexpectedToken("declaration statement");
    }

    auto declaration = std::make_unique<DeclarationStatementNode>(std::move(now_reading));

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

bool Parser::StartsAssignment(Token& token) {
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

bool Parser::StartsTargettableExpression(Token& token) {
    return StartsIdentifier(token)
        || StartsParenthesizedExpression(token);
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

bool Parser::StartsIfStatement(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {KeywordEnum::IF});
}
bool Parser::StartsElseBlock(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {KeywordEnum::ELSE});
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

bool Parser::StartsForStatement(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {KeywordEnum::FOR});
}
std::unique_ptr<ParseNode> Parser::ParseForStatement() {
    if (!StartsForStatement(*now_reading)) {
        return SyntaxErrorUnexpectedToken("for statement");
    }

    auto for_statement = std::make_unique<ForStatementNode>(std::move(now_reading));

    ReadToken();

    // Append a loop initializer node. If no initializer is specified, a noop is appended.
    std::unique_ptr<ParseNode> init = nullptr;
    if (StartsDeclaration(*now_reading)) {
        init = ParseDeclaration(/* expect_terminator = */ false);
    }
    else if (StartsAssignment(*now_reading)) {
        init = ParseAssignment(/* expect_terminator = */ false);
    }
    else {
        init = std::make_unique<NopNode>();
    }
    for_statement->AppendChild(std::move(init));

    Expect(PunctuatorEnum::SEPARATOR);

    // Append a loop condition node. If no condition is specified, condition is always true.
    std::unique_ptr<ParseNode> condition = nullptr;
    if (StartsExpression(*now_reading)) {
        condition = ParseExpression();
    }
    else {
        condition = std::make_unique<BooleanLiteralNode>(true);
    }
    for_statement->AppendChild(std::move(condition));

    Expect(PunctuatorEnum::SEPARATOR);

    // Append a loop increment node. If no increment is specified, a noop is appended.
    std::unique_ptr<ParseNode> increment = nullptr;
    if (StartsAssignment(*now_reading)) {
        increment = ParseAssignment(/* expect_terminator = */ false);
    }
    else {
        increment = std::make_unique<NopNode>();
    }
    for_statement->AppendChild(std::move(increment));

    // Append loop body.
    auto loop_body = ParseCodeBlock();
    for_statement->AppendChild(std::move(loop_body));

    return for_statement;
}

bool Parser::StartsPrintStatement(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {KeywordEnum::PRINT});
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

bool Parser::StartsPrintExpressionList(Token& token) {
    return StartsExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParsePrintExpressionList(std::unique_ptr<ParseNode> print_statement) {
    if (StartsExpression(*now_reading)) {
        std::unique_ptr<ParseNode> expression = ParseExpression();
        print_statement->AppendChild(std::move(expression));
    }
    else {
        return SyntaxErrorUnexpectedToken("printable expression");
    }

    while (!PunctuatorToken::IsTokenPunctuator(*now_reading, {PunctuatorEnum::TERMINATOR})
        && !now_reading->IsEOF()) {
        Expect(PunctuatorEnum::SEPARATOR);
        std::unique_ptr<ParseNode> expression = ParseExpression();
        print_statement->AppendChild(std::move(expression));
    }

    return print_statement;
}

bool Parser::StartsReturnStatement(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {KeywordEnum::RETURN});
}
std::unique_ptr<ParseNode> Parser::ParseReturnStatement() {
    if (!StartsReturnStatement(*now_reading)) {
        return SyntaxErrorUnexpectedToken("return statement");
    }

    auto return_statement = std::make_unique<ReturnStatementNode>(std::move(now_reading));
    // Discard 'return'.
    ReadToken();

    auto return_value = ParseExpression();
    return_statement->AppendChild(std::move(return_value));

    Expect(PunctuatorEnum::TERMINATOR);

    return return_statement;
}

bool Parser::StartsExpression(Token& token) {
    return StartsBooleanExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseExpression() {
    if (!StartsExpression(*now_reading)) {
        return SyntaxErrorUnexpectedToken("expression");
    }

    return ParseBooleanExpression();
}

bool Parser::StartsBooleanExpression(Token& token) {
    return StartsEqualityExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseBooleanExpression() {
    if (!StartsEqualityExpression(*now_reading)) {
        return SyntaxErrorUnexpectedToken("boolean expression");
    }

    return ParseEqualityExpression();
}

bool Parser::StartsEqualityExpression(Token& token) {
    return StartsComparisonExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseEqualityExpression() {
    if (!StartsEqualityExpression(*now_reading)) {
        return SyntaxErrorUnexpectedToken("equality expression");
    }

    auto lhs = ParseComparisonExpression();

    while (PunctuatorToken::IsTokenPunctuator(*now_reading,
        {PunctuatorEnum::CMP_EQ, PunctuatorEnum::CMP_NEQ})) {
        auto equality_operator = std::make_unique<OperatorNode>(std::move(now_reading));

        ReadToken();

        auto rhs = ParseComparisonExpression();

        equality_operator->AppendChild(std::move(lhs));
        equality_operator->AppendChild(std::move(rhs));
        lhs = std::move(equality_operator);
    }

    return lhs;
}

bool Parser::StartsComparisonExpression(Token& token) {
    return StartsAdditiveExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseComparisonExpression() {
    if (!StartsComparisonExpression(*now_reading)) {
        return SyntaxErrorUnexpectedToken("comparison expression");
    }

    auto lhs = ParseAdditiveExpression();

    while (PunctuatorToken::IsTokenPunctuator(*now_reading,
        {PunctuatorEnum::CMP_G, PunctuatorEnum::CMP_L,
        PunctuatorEnum::CMP_GEQ, PunctuatorEnum::CMP_LEQ})) {
        auto comparison_operator = std::make_unique<OperatorNode>(std::move(now_reading));

        ReadToken();

        auto rhs = ParseAdditiveExpression();

        comparison_operator->AppendChild(std::move(lhs));
        comparison_operator->AppendChild(std::move(rhs));
        lhs = std::move(comparison_operator);
    }

    return lhs;
}

bool Parser::StartsAdditiveExpression(Token& token) {
    return StartsMultiplicativeExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseAdditiveExpression() {
    if (!StartsAdditiveExpression(*now_reading)) {
        return SyntaxErrorUnexpectedToken("additive expression");
    }

    std::unique_ptr<ParseNode> left = ParseMultiplicativeExpression();

    while (PunctuatorToken::IsTokenPunctuator(*now_reading,
        {PunctuatorEnum::PLUS, PunctuatorEnum::MINUS})) {
        std::unique_ptr<ParseNode> additive_operator =
            std::make_unique<OperatorNode>(std::move(now_reading));
        
        ReadToken();

        std::unique_ptr<ParseNode> right = ParseMultiplicativeExpression();

        additive_operator->AppendChild(std::move(left));
        additive_operator->AppendChild(std::move(right));
        left = std::move(additive_operator);
    }

    return left;
}

bool Parser::StartsMultiplicativeExpression(Token& token) {
    return StartsUnaryExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseMultiplicativeExpression() {
    if (!StartsMultiplicativeExpression(*now_reading)) {
        return SyntaxErrorUnexpectedToken("multiplicative expression");
    }

    std::unique_ptr<ParseNode> left = ParseUnaryExpression();

    while (PunctuatorToken::IsTokenPunctuator(*now_reading,
        {PunctuatorEnum::MULTIPLY, PunctuatorEnum::DIVIDE})) {
        std::unique_ptr<ParseNode> multiplicative_operator =
            std::make_unique<OperatorNode>(std::move(now_reading));
        
        ReadToken();

        std::unique_ptr<ParseNode> right = ParseUnaryExpression();

        multiplicative_operator->AppendChild(std::move(left));
        multiplicative_operator->AppendChild(std::move(right));
        left = std::move(multiplicative_operator);
    }

    return left;
}

bool Parser::StartsUnaryExpression(Token& token) {
    return PunctuatorToken::IsTokenPunctuator(token, {PunctuatorEnum::PLUS, PunctuatorEnum::MINUS})
        || StartsAtomicExpression(token);
}
std::unique_ptr<ParseNode> Parser::ParseUnaryExpression() {
    if (!StartsUnaryExpression(*now_reading)) {
        return SyntaxErrorUnexpectedToken("unary expression");
    }

    if (PunctuatorToken::IsTokenPunctuator(*now_reading,
        {PunctuatorEnum::PLUS, PunctuatorEnum::MINUS})) {
        std::unique_ptr<ParseNode> unary_operator =
            std::make_unique<OperatorNode>(std::move(now_reading));
        
        ReadToken();

        std::unique_ptr<ParseNode> operand = ParseUnaryExpression();

        unary_operator->AppendChild(std::move(operand));

        return unary_operator;
    }
    else {
        return ParseAtomicExpression();
    }
}

bool Parser::StartsAtomicExpression(Token& token) {
    return StartsParenthesizedExpression(token)
            || StartsIdentifier(token)
            || StartsBooleanLiteral(token)
            || StartsCharacterLiteral(token)
            || StartsIntegerLiteral(token)
            || StartsStringLiteral(token);
        
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
    return SyntaxErrorUnexpectedToken("atomic expression");
}

bool Parser::StartsParenthesizedExpression(Token& token) {
    return PunctuatorToken::IsTokenPunctuator(token, {PunctuatorEnum::OPEN_PARENTHESIS});
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

bool Parser::StartsIdentifier(Token& token) {
    return token.GetTokenType() == TokenType::IDENTIFIER;
}
std::unique_ptr<ParseNode> Parser::ParseIdentifier() {
    if (!StartsIdentifier(*now_reading)) {
        return SyntaxErrorUnexpectedToken("identifier");
    }

    auto identifier = std::make_unique<IdentifierNode>(std::move(now_reading));

    ReadToken();

    return identifier;
}

bool Parser::StartsBooleanLiteral(Token& token) {
    return token.GetTokenType() == TokenType::BOOLEAN_LITERAL;
}
std::unique_ptr<ParseNode> Parser::ParseBooleanLiteral() {
    if (!StartsBooleanLiteral(*now_reading)) {
        return SyntaxErrorUnexpectedToken("boolean literal");
    }

    auto boolean_literal = std::make_unique<BooleanLiteralNode>(std::move(now_reading));
    ReadToken();
    return boolean_literal;
}

bool Parser::StartsCharacterLiteral(Token& token) {
    return token.GetTokenType() == TokenType::CHARACTER_LITERAL;
}
std::unique_ptr<ParseNode> Parser::ParseCharacterLiteral() {
    if (!StartsCharacterLiteral(*now_reading)) {
        return SyntaxErrorUnexpectedToken("character literal");
    }

    auto character_literal = std::make_unique<CharacterLiteralNode>(std::move(now_reading));
    ReadToken();
    return character_literal;
}

bool Parser::StartsIntegerLiteral(Token& token) {
    return token.GetTokenType() == TokenType::INTEGER_LITERAL;
}
std::unique_ptr<ParseNode> Parser::ParseIntegerLiteral() {
    if (!StartsIntegerLiteral(*now_reading)) {
        return SyntaxErrorUnexpectedToken("integer literal");
    }

    std::unique_ptr<ParseNode> integer_literal =
        std::make_unique<IntegerLiteralNode>(std::move(now_reading));
    ReadToken();
    return integer_literal;
}

bool Parser::StartsStringLiteral(Token& token) {
    return token.GetTokenType() == TokenType::STRING_LITERAL;
}
std::unique_ptr<ParseNode> Parser::ParseStringLiteral() {
    if (!StartsStringLiteral(*now_reading)) {
        return SyntaxErrorUnexpectedToken("string literal");
    }

    std::unique_ptr<StringLiteralNode> string_literal =
            std::make_unique<StringLiteralNode>(std::move(now_reading));
    ReadToken();
    return string_literal;
}

bool Parser::StartsType(Token& token) {
    return KeywordToken::IsTokenKeyword(token, {
        KeywordEnum::BOOL,
        KeywordEnum::CHAR,
        KeywordEnum::INT,
        KeywordEnum::STRING
    });
}
std::unique_ptr<ParseNode> Parser::ParseType() {
    if (!StartsType(*now_reading)) {
        return SyntaxErrorUnexpectedToken("type");
    }

    auto type_node = std::make_unique<TypeNode>(std::move(now_reading));
    ReadToken();
    return type_node;
}

std::unique_ptr<ParseNode> Parser::SyntaxErrorUnexpectedToken(std::string expected) {
    std::string message = "Unexpected token \'"
        + now_reading->GetLexeme()
        + "\', expected "
        + expected
        + " at "
        + now_reading->GetLocation().ToString();
    PunktLogger::Log(LogType::PARSER, message);
    return GetSyntaxErrorNode();
}

std::unique_ptr<ParseNode> Parser::GetSyntaxErrorNode() {
    std::unique_ptr<ParseNode> error = std::make_unique<ErrorNode>(std::move(now_reading));
    ReadToken();
    return error;
}
