#include <code_generator/code_generation_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include "operator_node.h"

OperatorNode::OperatorNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::OPERATOR_NODE, std::move(token))
{
    PunctuatorToken& punctuator_token = dynamic_cast<PunctuatorToken&>(*(this->token));
    punctuator_enum = punctuator_token.GetPunctuatorEnum();
}

std::string OperatorNode::ToString() const {
    return "OPERATOR NODE: " + token->ToString();
}

void OperatorNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *OperatorNode::GenerateCode(CodeGenerationVisitor &visitor) {
    return visitor.GenerateCode(*this);
}
