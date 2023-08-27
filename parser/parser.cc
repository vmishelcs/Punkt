#include <token/all_tokens.h>
#include <parse_node/all_nodes.h>
#include <logging/punkt_logger.h>

#include "parser.h"

Parser::Parser(Scanner& scanner) : scanner(scanner) {
    ReadToken();
}

void Parser::ReadToken() {
    now_reading = scanner.Next();
}

std::unique_ptr<ParseNode> Parser::Parse(Scanner& scanner) {
    Parser parser(scanner);
    return parser.ParseProgram();
}

bool Parser::StartsProgram(Token& token) {
    return StartsMain(token);
}

std::unique_ptr<ParseNode> Parser::ParseProgram() {
    if (!StartsProgram(*now_reading)) {
        return GetSyntaxErrorNode("well defined program start");
    }

    std::unique_ptr<ProgramToken> program_token = std::make_unique<ProgramToken>();
    std::unique_ptr<ParseNode> program = std::make_unique<ProgramNode>(std::move(program_token));

    std::unique_ptr<ParseNode> main = ParseMain();
    program->AppendChild(std::move(main));

    if (now_reading->GetTokenType() != TokenType::EOF_TOKEN) {
        return GetSyntaxErrorNode("end of program");
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
        return GetSyntaxErrorNode("main");
    }

    return nullptr;
}

std::unique_ptr<ParseNode> Parser::GetSyntaxErrorNode(std::string expected) {
    auto& logger = PunktLogger::GetInstance();
    std::string message = "Expected " + expected;
    logger.Log(LogType::PARSER, message);
    return std::make_unique<ErrorNode>(std::move(now_reading));
}
