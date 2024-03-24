#include <code_generator/code_generation_visitor.h>
#include <parse_node/parse_node_visitor.h>

#include "code_block_node.h"

CodeBlockNode::CodeBlockNode(std::unique_ptr<Token> token)
    : ParseNode(ParseNodeType::CODE_BLOCK_NODE, std::move(token))
{}

std::string CodeBlockNode::AsString() const {
    return "CODE BLOCK NODE: " + token->AsString();
}

void CodeBlockNode::Accept(ParseNodeVisitor& visitor) {
    visitor.VisitEnter(*this);
    VisitChildren(visitor);
    visitor.VisitLeave(*this);
}

llvm::Value *CodeBlockNode::GenerateCode(CodeGenerationVisitor& visitor) {
    return visitor.GenerateCode(*this);
}
