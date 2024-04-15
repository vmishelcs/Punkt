#include <llvm/IR/Value.h>

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>

#include "lambda_parameter_node.h"

std::unique_ptr<LambdaParameterNode> LambdaParameterNode::CreateParameterNode(
            std::unique_ptr<ParseNode> type,
            std::unique_ptr<ParseNode> identifier)
{
    auto function_parameter_node = std::make_unique<LambdaParameterNode>();
    function_parameter_node->AppendChild(std::move(type));
    function_parameter_node->AppendChild(std::move(identifier));
    return function_parameter_node;
}

void LambdaParameterNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *LambdaParameterNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
