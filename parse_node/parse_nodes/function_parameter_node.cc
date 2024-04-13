#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>

#include "function_parameter_node.h"

FunctionParameterNode::FunctionParameterNode()
    : ParseNode(ParseNodeType::FUNCTION_PARAMETER_NODE, nullptr)
{}

std::unique_ptr<FunctionParameterNode> FunctionParameterNode::CreateParameterNode(
            std::unique_ptr<ParseNode> type,
            std::unique_ptr<ParseNode> identifier)
{
    auto function_parameter_node = std::make_unique<FunctionParameterNode>();
    function_parameter_node->AppendChild(std::move(type));
    function_parameter_node->AppendChild(std::move(identifier));
    return function_parameter_node;
}

std::string FunctionParameterNode::ToString() const {
    return "FUNCTION PARAMETER NODE";
}

void FunctionParameterNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *FunctionParameterNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
