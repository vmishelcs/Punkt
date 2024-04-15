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

LambdaNode *ReturnStatementNode::GetEnclosingLambdaNode() const {
    // TODO: Think about what to do if the return statement is inside of 'main'.
    ParseNode *parse_node = GetParent();
    while (parse_node && parse_node->GetParseNodeType() != ParseNodeType::LAMBDA_NODE) {
        parse_node = parse_node->GetParent();
    }

    if (!parse_node) {
        PunktLogger::LogFatalInternalError("return statement does not have enclosing lambda");
    }

    auto lambda_node = dynamic_cast<LambdaNode *>(parse_node);
    if (!lambda_node) {
        PunktLogger::LogFatalInternalError(
                "failed casting ParseNode with ParseNodeType == LAMBDA_NODE to LambdaNode");
    }

    return lambda_node;
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
