#include "base_type_node.h"

#include <llvm/IR/Value.h>
#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <scanner/keyword.h>
#include <semantic_analyzer/types/base_type.h>
#include <token/keyword_token.h>

#include <memory>
#include <string>

std::unique_ptr<ParseNode> BaseTypeNode::CreateCopy() const {
  return std::make_unique<BaseTypeNode>(token->CreateCopy());
}

std::unique_ptr<Type> BaseTypeNode::InferOwnType() const {
  Token *token = GetToken();
  KeywordToken *keyword_token = dynamic_cast<KeywordToken *>(token);
  if (!keyword_token) {
    PunktLogger::LogFatalInternalError(
        "BaseTypeNode::GetToken returned a non-keyword token");
  }

  switch (keyword_token->GetKeywordEnum()) {
    case Keyword::VOID:
      return BaseType::CreateVoidType();
    case Keyword::BOOL:
      return BaseType::CreateBooleanType();
    case Keyword::CHAR:
      return BaseType::CreateCharacterType();
    case Keyword::INT:
      return BaseType::CreateIntegerType();
    case Keyword::FLOAT:
      return BaseType::CreateFloatType();
    case Keyword::RAT:
      return BaseType::CreateRationalType();
    case Keyword::STRING:
      return BaseType::CreateStringType();
    default:
      PunktLogger::LogFatalInternalError(
          "BaseTypeNode keyword does not name a known type");
  }
}

void BaseTypeNode::Accept(ParseNodeVisitor &visitor) { visitor.Visit(*this); }

llvm::Value *BaseTypeNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
