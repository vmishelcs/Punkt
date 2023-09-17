#include <parse_node/parse_node_visitor.h>

#include "print_statement_node.h"

PrintStatementNode::PrintStatementNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::PRINT_STATEMENT_NODE, std::move(token))
{}

std::string PrintStatementNode::AsString() const {
    return "PRINT STATEMENT NODE: " + token->AsString();
}

void PrintStatementNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}
