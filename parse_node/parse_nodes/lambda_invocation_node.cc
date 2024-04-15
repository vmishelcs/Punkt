#include <iterator>
#include <memory>
#include <vector>

#include <llvm/IR/Value.h>

#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include "lambda_invocation_node.h"

std::unique_ptr<LambdaInvocationNode> LambdaInvocationNode::CreateLambdaInvocationNodeWithArguments(
        std::unique_ptr<ParseNode> identifier,
        std::vector<std::unique_ptr<ParseNode>> args)
{
    auto lambda_invocation = std::make_unique<LambdaInvocationNode>();
    lambda_invocation->AppendChild(std::move(identifier));
    
    for (unsigned i = 0, n = args.size(); i < n; ++i) {
        lambda_invocation->AppendChild(std::move(args.at(i)));
    }

    return lambda_invocation;
}

std::vector<ParseNode *> LambdaInvocationNode::GetArgumentNodes() const {
    std::vector<ParseNode *> result;
    if (NumChildren() == 1) {
        return result;
    }

    auto children = GetChildren();
    for (unsigned i = 1, n = NumChildren(); i < n; ++i) {
        result.push_back(children[i]);
    }
    return result;
}

void LambdaInvocationNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *LambdaInvocationNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
