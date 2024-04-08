#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include "if_statement_node.h"

IfStatementNode::IfStatementNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::IF_STATEMENT_NODE, std::move(token))
{}

bool IfStatementNode::HasElseBlock() const {
    // If-statement has an 'else' block if there are more than two children (first 2 children are 
    // the condition and the 'then' block).
    return NumChildren() > 2;
}

std::string IfStatementNode::ToString() const {
    return "IF STATEMENT NODE: " + token->ToString();
}

void IfStatementNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *IfStatementNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
