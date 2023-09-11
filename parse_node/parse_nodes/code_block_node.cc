#include <parse_node/parse_node_visitor.h>

#include "code_block_node.h"

CodeBlockNode::CodeBlockNode(std::unique_ptr<Token> token)
    : ParseNode(std::move(token))
{}

std::string CodeBlockNode::GetNodeString() {
    return "CODE BLOCK NODE: " + token->GetTokenString();
}

void CodeBlockNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}
