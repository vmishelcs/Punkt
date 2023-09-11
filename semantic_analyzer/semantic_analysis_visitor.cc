#include "semantic_analysis_visitor.h"

#include "primitive_type.h"

// Non-leaf nodes
void SemanticAnalysisVisitor::VisitEnter(CodeBlockNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(CodeBlockNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(DeclarationStatementNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(DeclarationStatementNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(MainNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(MainNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(OperatorNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(OperatorNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(PrintStatementNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(PrintStatementNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(ProgramNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(ProgramNode& node) {

}

// Leaf nodes
void SemanticAnalysisVisitor::Visit(ErrorNode& node) {
    
}
void SemanticAnalysisVisitor::Visit(IdentifierNode& node) {
    // Analysis done by parent node
}
void SemanticAnalysisVisitor::Visit(IntegerLiteralNode& node) {
    
}
