#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <semantic_analyzer/types/lambda_type.h>

#include "lambda_type_node.h"

std::unique_ptr<Type> LambdaTypeNode::InferOwnType() const {
    std::vector<Type *> parameter_types;
    parameter_types.reserve(parameter_type_nodes.size());

    // Fill parameter types vector with corresponding types.
    std::transform(parameter_type_nodes.begin(), parameter_type_nodes.end(),
            std::inserter(parameter_types, parameter_types.end()),
            [](const auto& param_node) { return param_node->GetType(); });
    
    return LambdaType::CreateLambdaType(parameter_types, return_type_node->GetType());
}

void LambdaTypeNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *LambdaTypeNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
