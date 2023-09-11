#include "semantic_analysis_visitor.h"

#include "type.h"

// Non-leaf nodes
void SemanticAnalysisVisitor::VisitEnter(CodeBlockNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(CodeBlockNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(DeclarationStatementNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::VisitLeave(DeclarationStatementNode& node) {
    
}

void SemanticAnalysisVisitor::VisitEnter(MainNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::VisitLeave(MainNode& node) {
    // Do nothing
}

void SemanticAnalysisVisitor::VisitEnter(OperatorNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::VisitLeave(OperatorNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(PrintStatementNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::VisitLeave(PrintStatementNode& node) {
    // Do nothing
}

void SemanticAnalysisVisitor::VisitEnter(ProgramNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::VisitLeave(ProgramNode& node) {
    // Do nothing
}

// Leaf nodes
void SemanticAnalysisVisitor::Visit(ErrorNode& node) {
    node.SetType(Type(TypeEnum::ERROR));
}
void SemanticAnalysisVisitor::Visit(IdentifierNode& node) {
    // Analysis done by parent node
}
void SemanticAnalysisVisitor::Visit(IntegerLiteralNode& node) {
    node.SetType(Type(TypeEnum::INTEGER));
}
