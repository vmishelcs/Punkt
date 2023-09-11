#include <parse_node/parse_node_visitor.h>

#include "main_node.h"

MainNode::MainNode(std::unique_ptr<Token> token)
    : ParseNode(std::move(token))
{}

std::string MainNode::GetNodeString() {
    return "MAIN NODE: " + token->GetTokenString();
}

void MainNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}
