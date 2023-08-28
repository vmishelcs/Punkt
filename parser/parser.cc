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

    Expect(PunctuatorEnum::CLOSE_BRACE);

    return main;
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
