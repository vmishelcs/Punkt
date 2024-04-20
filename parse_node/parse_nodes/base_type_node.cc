#include <string>

#include <llvm/IR/Value.h>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>
#include <scanner/keyword.h>
#include <semantic_analyzer/types/base_type.h>
#include <token/keyword_token.h>

#include "base_type_node.h"

std::unique_ptr<Type> BaseTypeNode::InferOwnType() const {
    Token *token = GetToken();
    KeywordToken *keyword_token = dynamic_cast<KeywordToken *>(token);
    if (!keyword_token) {
        PunktLogger::LogFatalInternalError("BaseTypeNode::GetToken returned a non-keyword token");
    }

    switch (keyword_token->GetKeywordEnum()) {
        case KeywordEnum::VOID:
            return BaseType::Create(BaseTypeEnum::VOID);
        case KeywordEnum::BOOL:
            return BaseType::Create(BaseTypeEnum::BOOLEAN);
        case KeywordEnum::CHAR:
            return BaseType::Create(BaseTypeEnum::CHARACTER);
        case KeywordEnum::INT:
            return BaseType::Create(BaseTypeEnum::INTEGER);
        case KeywordEnum::STRING:
            return BaseType::Create(BaseTypeEnum::STRING);
        default:
            PunktLogger::LogFatalInternalError("BaseTypeNode keyword does not name a known type");
            return nullptr;
    }
}

void BaseTypeNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}

llvm::Value *BaseTypeNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
