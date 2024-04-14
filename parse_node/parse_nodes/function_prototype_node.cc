#include <algorithm>
#include <iterator>
#include <vector>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>

#include "function_prototype_node.h"

FunctionPrototypeNode::FunctionPrototypeNode()
    : ParseNode(ParseNodeType::FUNCTION_PROTOTYPE_NODE, nullptr)
{}

std::vector<Type *> FunctionPrototypeNode::GetParameterTypes() const {
    std::vector<Type *> result;

    // If there is only one child node, then there are no parameters.
    if (NumChildren() == 1) {
        return result;
    }

    result.reserve(NumChildren() - 1);

    auto children = GetChildren();
    std::transform(children.begin(), --children.end(),
            std::inserter(result, result.end()),
            [](const auto& parameter_node) { return parameter_node->GetType(); });

    return result;
}

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
