#include "parse_node.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <logging/punkt_logger.h>
#include <semantic_analyzer/types/base_type.h>
#include <semantic_analyzer/types/lambda_type.h>
#include <semantic_analyzer/types/type.h>

#include <memory>
#include <vector>

#include "parse_node_visitor.h"

ParseNode::ParseNode(ParseNodeType node_type, TextLocation &text_location)
    : token{nullptr},
      node_type(node_type),
      text_location(text_location),
      parent{nullptr},
      type{nullptr},
      scope{nullptr} {}

ParseNode::ParseNode(ParseNodeType node_type, std::unique_ptr<Token> token)
    : token(std::move(token)),
      node_type(node_type),
      text_location(this->token->GetLocation()),
      parent{nullptr},
      type{nullptr},
      scope{nullptr} {}

std::unique_ptr<ParseNode> ParseNode::CreateCopy() {
  // return std::make_unique<ParseNode>(this->node_type, );
  return nullptr;
}

Token *ParseNode::GetToken() const { return token.get(); }

ParseNode *ParseNode::GetChild(unsigned i) const {
  if (children.size() <= i) {
    return nullptr;
  }
  return children[i].get();
}

unsigned ParseNode::NumChildren() const { return children.size(); }

std::vector<ParseNode *> ParseNode::GetChildren() const {
  std::vector<ParseNode *> result;
  for (unsigned i = 0, n = children.size(); i < n; ++i) {
    result.push_back(children[i].get());
  }
  return result;
}

void ParseNode::AppendChild(std::unique_ptr<ParseNode> node) {
  node->parent = this;
  children.push_back(std::move(node));
}

std::vector<ParseNode *> ParseNode::GetPathToRoot() {
  std::vector<ParseNode *> result;
  ParseNode *current = this;
  while (current) {
    result.push_back(current);
    current = current->parent;
  }
  return result;
}

bool ParseNode::HasType() const { return type != nullptr; }

Type *ParseNode::GetType() const { return type.get(); }

void ParseNode::SetType(std::unique_ptr<Type> type) {
  this->type = std::move(type);
}

bool ParseNode::HasScope() const { return scope != nullptr; }

Scope *ParseNode::GetScope() const { return scope.get(); }

void ParseNode::SetScope(std::unique_ptr<Scope> scope) {
  this->scope = std::move(scope);
}

Scope *ParseNode::GetLocalScope() {
  for (auto node : GetPathToRoot()) {
    if (node->HasScope()) {
      return node->GetScope();
    }
  }
  return nullptr;
}

llvm::Type *ParseNode::GetLLVMType(llvm::LLVMContext &context) const {
  Type *type = GetType();
  auto base_type = dynamic_cast<BaseType *>(type);
  if (base_type) {
    switch (base_type->GetBaseTypeEnum()) {
      case BaseTypeEnum::VOID:
        return llvm::Type::getVoidTy(context);
      case BaseTypeEnum::BOOLEAN:
        return llvm::Type::getInt8Ty(context);
      case BaseTypeEnum::CHARACTER:
        return llvm::Type::getInt8Ty(context);
      case BaseTypeEnum::INTEGER:
        return llvm::Type::getInt32Ty(context);
      case BaseTypeEnum::STRING:
        return llvm::PointerType::getUnqual(context);
      default:
        return (llvm::Type *)PunktLogger::LogFatalInternalError(
            "unimplemented BaseType in ParseNode::GetLLVMType");
    }
  }
  auto lambda_type = dynamic_cast<LambdaType *>(type);
  if (lambda_type) {
    return llvm::PointerType::getUnqual(context);
  }
  return (llvm::Type *)PunktLogger::LogFatalInternalError(
      "ParseNode::GetLLVMType implemented only for BaseType and LambdaType");
}

void ParseNode::VisitChildren(ParseNodeVisitor &visitor) {
  for (auto &child : children) {
    child->Accept(visitor);
  }
}
