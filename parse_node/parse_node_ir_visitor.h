#ifndef PARSE_NODE_IR_VISITOR_H_
#define PARSE_NODE_IR_VISITOR_H_

#include <llvm/IR/Value.h>

#include "parse_nodes/all_nodes.h"

class ParseNodeIRVisitor {
public:
    virtual llvm::Value *GenerateCode(AssignmentStatementNode& node) = 0;
    virtual llvm::Value *GenerateCode(CodeBlockNode& node) = 0;
    virtual llvm::Value *GenerateCode(DeclarationStatementNode& node) = 0;
    virtual llvm::Value *GenerateCode(ForStatementNode& node) = 0;
    virtual llvm::Value *GenerateCode(FunctionDefinitionNode& node) = 0;
    virtual llvm::Value *GenerateCode(LambdaParameterNode& node) = 0;
    virtual llvm::Value *GenerateCode(IfStatementNode& node) = 0;
    virtual llvm::Value *GenerateCode(LambdaInvocationNode& node) = 0;
    virtual llvm::Value *GenerateCode(LambdaNode& node) = 0;
    virtual llvm::Value *GenerateCode(MainNode& node) = 0;
    virtual llvm::Value *GenerateCode(OperatorNode& node) = 0;
    virtual llvm::Value *GenerateCode(PrintStatementNode& node) = 0;
    virtual llvm::Value *GenerateCode(ProgramNode& node) = 0;
    virtual llvm::Value *GenerateCode(ReturnStatementNode& node) = 0;
    
    virtual llvm::Value *GenerateCode(ErrorNode& node) = 0;
    virtual llvm::Value *GenerateCode(NopNode& node) = 0;
    virtual llvm::Value *GenerateCode(IdentifierNode& node) = 0;
    virtual llvm::Value *GenerateCode(BooleanLiteralNode& node) = 0;
    virtual llvm::Value *GenerateCode(CharacterLiteralNode& node) = 0;
    virtual llvm::Value *GenerateCode(IntegerLiteralNode& node) = 0;
    virtual llvm::Value *GenerateCode(StringLiteralNode& node) = 0;
    virtual llvm::Value *GenerateCode(TypeNode& node) = 0;
};

#endif // PARSE_NODE_IR_VISITOR_H_
