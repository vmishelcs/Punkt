#include "character_literal_node.h"

#include <parse_node/parse_node.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/character_literal_token.h>

CharacterLiteralNode::CharacterLiteralNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::CHARACTER_LITERAL_NODE, std::move(token)) {}

char CharacterLiteralNode::GetValue() const {
  CharacterLiteralToken& character_literal_token =
      dynamic_cast<CharacterLiteralToken&>(*(this->token));
  return character_literal_token.GetValue();
}

std::string CharacterLiteralNode::ToString() const {
  return "CHARACTER LITERAL NODE: " + token->ToString();
}

void CharacterLiteralNode::Accept(ParseNodeVisitor& visitor) {
  visitor.Visit(*this);
}

llvm::Value* CharacterLiteralNode::GenerateCode(ParseNodeIRVisitor& visitor) {
  return visitor.GenerateCode(*this);
}
