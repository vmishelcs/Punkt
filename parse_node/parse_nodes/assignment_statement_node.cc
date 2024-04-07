#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>

#include "assignment_statement_node.h"

AssignmentStatementNode::AssignmentStatementNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::ASSIGNMENT_STATEMENT_NODE, std::move(token))
{}

std::string AssignmentStatementNode::ToString() const {
    return "ASSIGNMENT STATEMENT NODE: " + token->ToString();
}

void AssignmentStatementNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *AssignmentStatementNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
