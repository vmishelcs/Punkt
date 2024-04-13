#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include "function_node.h"

FunctionNode::FunctionNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::FUNCTION_NODE, std::move(token))
{}

std::string FunctionNode::ToString() const {
    return "FUNCTION NODE";
}

void FunctionNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *FunctionNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
