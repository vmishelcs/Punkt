#include <token/all_tokens.h>
#include <parse_node/all_nodes.h>
#include <logging/punkt_logger.h>

#include "parser.h"

Parser::Parser(Scanner& scanner) : scanner(scanner) {
    ReadToken();
}

std::unique_ptr<ParseNode> Parser::Parse(Scanner& scanner) {
    Parser parser(scanner);
    return parser.ParseProgram();
}

void Parser::ReadToken() {
    now_reading = nullptr;
    now_reading = scanner.Next();
    if (!now_reading) {
        std::runtime_error("Failed to read next token in Parser::ReadToken");
    }
}

void Parser::Expect(KeywordEnum keyword) {
    if (now_reading->GetTokenType() != TokenType::KEYWORD) {
        SyntaxErrorUnexpectedToken(Keyword::ForKeywordEnum(keyword));
        ReadToken();
        return;
    }

    KeywordToken& keyword_token = dynamic_cast<KeywordToken&>(*now_reading);
    if (keyword_token.GetKeywordEnum() != keyword) {
        SyntaxErrorUnexpectedToken(Keyword::ForKeywordEnum(keyword));
        ReadToken();
        return;
    }

    ReadToken();
}

void Parser::Expect(PunctuatorEnum punctuator) {
    if (now_reading->GetTokenType() != TokenType::PUNCTUATOR) {
        SyntaxErrorUnexpectedToken(Punctuator::ForPunctuatorEnum(punctuator));
        ReadToken();
        return;
    }

    PunctuatorToken& punctuator_token = dynamic_cast<PunctuatorToken&>(*now_reading);
    if (punctuator_token.GetPunctuatorEnum() != punctuator) {
        SyntaxErrorUnexpectedToken(Punctuator::ForPunctuatorEnum(punctuator));
        ReadToken();
        return;
    }

    ReadToken();
}

bool Parser::StartsProgram(Token& token) {
    return StartsMain(token);
}

std::unique_ptr<ParseNode> Parser::ParseProgram() {
    if (!StartsProgram(*now_reading)) {
        return SyntaxErrorUnexpectedToken("well defined program start");
    }

    std::unique_ptr<ProgramToken> program_token = std::make_unique<ProgramToken>();
    std::unique_ptr<ParseNode> program = std::make_unique<ProgramNode>(std::move(program_token));

    // Parse global variables here.

    std::unique_ptr<ParseNode> main = ParseMain();
    program->AppendChild(std::move(main));

    if (now_reading->GetTokenType() != TokenType::EOF_TOKEN) {
        return SyntaxErrorUnexpectedToken("end of program");
    }

    return program;
}

bool Parser::StartsMain(Token& token) {
    if (token.GetTokenType() != TokenType::KEYWORD) {
        return false;
    }

    KeywordToken& keyword_token = dynamic_cast<KeywordToken&>(token);
    return keyword_token.GetKeywordEnum() == KeywordEnum::MAIN;
}

std::unique_ptr<ParseNode> Parser::ParseMain() {
    if (!StartsMain(*now_reading)) {
        return SyntaxErrorUnexpectedToken("main");
    }

    std::unique_ptr<MainNode> main = std::make_unique<MainNode>(std::move(now_reading));
    ReadToken();

    Expect(PunctuatorEnum::OPEN_BRACE);

    while (StartsStatement(*now_reading)) {
        std::unique_ptr<ParseNode> statement = ParseStatement();
        main->AppendChild(std::move(statement));
    }

    Expect(PunctuatorEnum::CLOSE_BRACE);

    return main;
}

bool Parser::StartsStatement(Token& token) {
    return StartsDeclaration(token);
}

std::unique_ptr<ParseNode> Parser::ParseStatement() {
    if (StartsDeclaration(*now_reading)) {
        return ParseDeclaration();
    }
    else {
        return SyntaxErrorUnexpectedToken("start of statement");
    }
}

bool Parser::StartsDeclaration(Token& token) {
    if (token.GetTokenType() != TokenType::KEYWORD) {
        return false;
    }

    KeywordToken& keyword_token = dynamic_cast<KeywordToken&>(token);
    return keyword_token.GetKeywordEnum() == KeywordEnum::VAR;
}

std::unique_ptr<ParseNode> Parser::ParseDeclaration() {
    if (!StartsDeclaration(*now_reading)) {
        return SyntaxErrorUnexpectedToken("start of statement");
    }

    std::unique_ptr<ParseNode> declaration =
        std::make_unique<DeclarationNode>(std::move(now_reading));
    ReadToken();

    std::unique_ptr<ParseNode> identifier = ParseIdentifier();

    Expect(PunctuatorEnum::EQUAL);

    std::unique_ptr<ParseNode> initializer = ParseExpression();

    Expect(PunctuatorEnum::TERMINATOR);

    declaration->AppendChild(std::move(identifier));
    declaration->AppendChild(std::move(initializer));

    return declaration;
}

bool Parser::StartsIdentifier(Token& token) {
    return token.GetTokenType() == TokenType::IDENTIFIER;
}

std::unique_ptr<ParseNode> Parser::ParseIdentifier() {
    if (!StartsIdentifier(*now_reading)) {
        return SyntaxErrorUnexpectedToken("identifier");
    }

    std::unique_ptr<ParseNode> identifier =
        std::make_unique<IdentifierNode>(std::move(now_reading));
    ReadToken();

    return identifier;
}

bool Parser::StartsExpression(Token& token) {
    return StartsAdditiveExpression(token);
}

std::unique_ptr<ParseNode> Parser::ParseExpression() {
    if (!StartsExpression(*now_reading)) {
        return SyntaxErrorUnexpectedToken("expression");
    }

    return ParseAdditiveExpression();
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

    // TODO: This code should be able to parse an indefinite number of unary operators
    if (PunctuatorToken::IsTokenPunctuator(*now_reading,
        {PunctuatorEnum::PLUS, PunctuatorEnum::MINUS})) {
        std::unique_ptr<ParseNode> unary_operator =
            std::make_unique<OperatorNode>(std::move(now_reading));
        
        ReadToken();

        std::unique_ptr<ParseNode> operand = ParseAtomicExpression();

        unary_operator->AppendChild(std::move(operand));

        return unary_operator;
    }
    else {
        return ParseAtomicExpression();
    }
}

bool Parser::StartsAtomicExpression(Token& token) {
    return StartsParenthesizedExpression(token)
        || StartsIntegerLiteral(token)
        || StartsIdentifier(token);
}

std::unique_ptr<ParseNode> Parser::ParseAtomicExpression() {
    if (StartsParenthesizedExpression(*now_reading)) {
        return ParseParenthesizedExpression();
    }
    if (StartsIntegerLiteral(*now_reading)) {
        return ParseIntegerLiteral();
    }
    if (StartsIdentifier(*now_reading)) {
        return ParseIdentifier();
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

std::unique_ptr<ParseNode> Parser::SyntaxErrorUnexpectedToken(std::string expected) {
    auto& logger = PunktLogger::GetInstance();
    std::string message = "Unexpected token "
        + now_reading->GetLexeme()
        + ", expected "
        + expected;
    logger.Log(LogType::PARSER, message);
    return GetSyntaxErrorNode();
}

std::unique_ptr<ParseNode> Parser::GetSyntaxErrorNode() {
    return std::make_unique<ErrorNode>(std::move(now_reading));
}
