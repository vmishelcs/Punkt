#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>

#include "type_node.h"

TypeNode::TypeNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::TYPE_NODE, std::move(token))
{}

std::string TypeNode::ToString() const {
    return "TYPE NODE";
}

void TypeNode::Accept(ParseNodeVisitor& visitor) {
    visitor.Visit(*this);
}

llvm::Value *TypeNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
