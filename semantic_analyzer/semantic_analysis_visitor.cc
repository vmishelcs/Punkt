#include "semantic_analysis_visitor.h"

#include <symbol_table/scope.h>
#include <token/keyword_token.h>

#include "type.h"

// Non-leaf nodes
void SemanticAnalysisVisitor::VisitEnter(CodeBlockNode& node) {
    CreateSubscope(node);
}
void SemanticAnalysisVisitor::VisitLeave(CodeBlockNode& node) {
    // Do nothing
}

void SemanticAnalysisVisitor::VisitEnter(DeclarationStatementNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::VisitLeave(DeclarationStatementNode& node) {
    bool is_mutable = KeywordToken::IsTokenKeyword(node.GetToken(), {KeywordEnum::VAR});

    IdentifierNode& identifier = dynamic_cast<IdentifierNode&>(node.GetChild(0));
    ParseNode& initializer = node.GetChild(1);

    TypeEnum declaration_type = initializer.GetType().GetTypeEnum();

    identifier.SetType(std::make_unique<Type>(declaration_type));

    Declare(identifier, is_mutable, declaration_type);
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
    CreateGlobalScope(node);
}
void SemanticAnalysisVisitor::VisitLeave(ProgramNode& node) {
    // Do nothing
}

// Leaf nodes
void SemanticAnalysisVisitor::Visit(ErrorNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
}
void SemanticAnalysisVisitor::Visit(IdentifierNode& node) {
    // Do nothing for now
}
void SemanticAnalysisVisitor::Visit(IntegerLiteralNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::INTEGER));
}

// Scoping
void SemanticAnalysisVisitor::CreateGlobalScope(ProgramNode& node) {
    node.SetScope(Scope::CreateGlobalScope());
}
void SemanticAnalysisVisitor::CreateSubscope(CodeBlockNode& node) {
    Scope& local_scope = node.GetLocalScope();
    node.SetScope(local_scope.CreateSubscope());
}

void SemanticAnalysisVisitor::Declare(IdentifierNode& node, bool is_mutable, TypeEnum type_enum) {
    Scope& local_scope = node.GetLocalScope();
    local_scope.DeclareInScope(
        node.GetToken().GetLexeme(),
        node.GetToken().GetLocation(),
        is_mutable,
        type_enum
    );
}
