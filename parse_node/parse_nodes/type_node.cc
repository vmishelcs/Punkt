#include <string>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>
#include <scanner/keyword.h>
#include <semantic_analyzer/types/base_type.h>
#include <semantic_analyzer/types/type.h>
#include <token/keyword_token.h>

#include "type_node.h"

TypeNode::TypeNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::TYPE_NODE, std::move(token))
{}

void TypeNode::InferOwnType() {
    Token *type_node_token = GetToken();
    KeywordToken *keyword_token = dynamic_cast<KeywordToken *>(type_node_token);
    if (!keyword_token) {
        PunktLogger::LogFatalInternalError("TypeNode::GetToken returned a non-keyword token");
    }

    switch (keyword_token->GetKeywordEnum()) {
        case KeywordEnum::BOOL:
            SetType(BaseType::Create(BaseTypeEnum::BOOLEAN));
            break;
        case KeywordEnum::CHAR:
            SetType(BaseType::Create(BaseTypeEnum::CHARACTER));
            break;
        case KeywordEnum::INT:
            SetType(BaseType::Create(BaseTypeEnum::INTEGER));
            break;
        case KeywordEnum::STRING:
            SetType(BaseType::Create(BaseTypeEnum::STRING));
            break;
        default:
            PunktLogger::LogFatalInternalError("TypeNode keyword does not name a known type");
    }
}

std::string TypeNode::ToString() const {
    return "TYPE NODE";
}

void TypeNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}

llvm::Value *TypeNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
