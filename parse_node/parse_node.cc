#include "parse_node.h"
#include "parse_node_visitor.h"

ParseNode::ParseNode(std::unique_ptr<Token> token)
    : token(std::move(token))
    , parent{nullptr}
{}

const std::vector<std::unique_ptr<ParseNode>>& ParseNode::GetChildren() const {
    return children;
}

void ParseNode::AppendChild(std::unique_ptr<ParseNode> node) {
    node->parent = this;
    children.push_back(std::move(node));
}

void ParseNode::VisitChildren(ParseNodeVisitor& visitor) {
    for (const auto& child : children) {
        child->Accept(visitor);
    }
}
