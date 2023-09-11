#include <parse_node/parse_node_visitor.h>

#include "print_statement_node.h"

PrintStatementNode::PrintStatementNode(std::unique_ptr<Token> token)
    : ParseNode(std::move(token))
{}

std::string PrintStatementNode::GetNodeString() {
    return "PRINT STATEMENT NODE: " + token->GetTokenString();
}

void PrintStatementNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}
