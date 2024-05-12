#include "identifier_node.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <logging/punkt_logger.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <token/operator_token.h>
#include <token/token.h>

#include "operator_node.h"

IdentifierNode::IdentifierNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::IDENTIFIER_NODE, std::move(token)),
      name(std::string(this->token->GetLexeme())),
      symbol_table_entry{nullptr} {}

std::unique_ptr<ParseNode> IdentifierNode::CreateCopy() const {
  return std::make_unique<IdentifierNode>(token->CreateCopy());
}

std::string IdentifierNode::ToString() const {
  return "IDENTIFIER NODE: " + token->ToString();
}

void IdentifierNode::Accept(ParseNodeVisitor &visitor) { visitor.Visit(*this); }

SymbolTableEntry *IdentifierNode::FindSymbolTableEntry() {
  std::string identifier = GetName();
  Scope *scope = this->GetLocalScope();
  while (scope) {
    if (scope->Declares(identifier)) {
      return scope->GetSymbolTableEntry(identifier);
    }
    scope = scope->GetBaseScope();
  }
  return nullptr;
}

bool IdentifierNode::IsAssignmentTarget() const {
  ParseNode *parent = GetParent();
  auto op_node = dynamic_cast<OperatorNode *>(parent);
  if (op_node && op_node->GetOperatorEnum() == Operator::ASSIGN &&
      op_node->GetChild(0) == this) {
    return true;
  }
  return false;
}

void IdentifierNode::SetLLVMAlloca(llvm::AllocaInst *alloca) {
  if (!symbol_table_entry) {
    PunktLogger::LogFatalInternalError(
        "unset symbol table entry in IdentifierNode::SetLLVMAlloca");
  }
  symbol_table_entry->alloca = alloca;
}

void IdentifierNode::SetLLVMFunction(llvm::Function *function) {
  if (!symbol_table_entry) {
    PunktLogger::LogFatalInternalError(
        "unset symbol table entry in IdentifierNode::SetLLVMFunction");
  }
  symbol_table_entry->function = function;
}

llvm::Value *IdentifierNode::GenerateCode(ParseNodeIRVisitor &visitor) {
  return visitor.GenerateCode(*this);
}
