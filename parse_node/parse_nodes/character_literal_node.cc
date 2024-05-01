#include "character_literal_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/character_literal_token.h>

#include <memory>

CharacterLiteralNode::CharacterLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::CHARACTER_LITERAL_NODE, std::move(token)) {
  auto char_literal_token =
      static_cast<CharacterLiteralToken *>(this->token.get());
  this->value = char_literal_token->GetValue();
}

std::unique_ptr<ParseNode> CharacterLiteralNode::CreateCopy() const {
  return std::make_unique<CharacterLiteralNode>(token->CreateCopy());
}

std::string CharacterLiteralNode::ToString() const {
  return "CHARACTER LITERAL NODE: " + token->ToString();
}

void CharacterLiteralNode::Accept(ParseNodeVisitor &visitor) {
  visitor.Visit(*this);
}

llvm::Value *CharacterLiteralNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
