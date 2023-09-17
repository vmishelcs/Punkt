#include <parse_node/parse_node_visitor.h>

#include "program_node.h"

ProgramNode::ProgramNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::PROGRAM_NODE, std::move(token))
{}

std::string ProgramNode::GetNodeString() const {
    return "PROGRAM NODE: " + token->GetTokenString();
}

void ProgramNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}
