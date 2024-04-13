#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>

#include "function_prototype_node.h"

FunctionPrototypeNode::FunctionPrototypeNode()
    : ParseNode(ParseNodeType::FUNCTION_PROTOTYPE_NODE, nullptr)
{}

std::string FunctionPrototypeNode::ToString() const {
    return "FUNCTION PROTOTYPE NODE";
}

void FunctionPrototypeNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *FunctionPrototypeNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
