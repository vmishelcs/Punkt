#include <string>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node_ir_visitor.h>
#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_node.h>

#include "lambda_node.h"
#include "return_statement_node.h"

ReturnStatementNode::ReturnStatementNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::RETURN_STATEMENT_NODE, std::move(token))
{}

ParseNode *ReturnStatementNode::GetEnclosingFunctionNode() const {
    ParseNode *parse_node = GetParent();
    while (parse_node) {
        if (parse_node->GetParseNodeType() == ParseNodeType::LAMBDA_NODE)
            return parse_node;
        if (parse_node->GetParseNodeType() == ParseNodeType::MAIN_NODE)
            return parse_node;

        parse_node = parse_node->GetParent();
    }

    return nullptr;
}

std::string ReturnStatementNode::ToString() const {
    return "RETURN STATEMENT NODE: " + token->ToString();
}

void ReturnStatementNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *ReturnStatementNode::GenerateCode(ParseNodeIRVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
