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
    if (!IsBeingDeclared(node)) {
        std::optional<std::reference_wrapper<SymbolData>> symbol_data_opt
            = node.FindIdentifierSymbolData();

        if (!symbol_data_opt.has_value()) {
            SymbolTable::UndefinedSymbolReference(
                node.GetToken().GetLexeme(),
                node.GetToken().GetLocation()
            );
            node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
            Declare(node, false, TypeEnum::ERROR);
        }
        else {
            SymbolData& symbol_data = symbol_data_opt.value();
            node.SetType(std::make_unique<Type>(symbol_data.type_enum));
        }
    }
    // Otherwise, semantic analysis is handled by VisitLeave(DeclarationStatementNode&)
}
void SemanticAnalysisVisitor::Visit(IntegerLiteralNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::INTEGER));
}

// Miscellaneous helpers
void SemanticAnalysisVisitor::Declare(IdentifierNode& node, bool is_mutable, TypeEnum type_enum) {
    Scope& local_scope = node.GetLocalScope().value().get();
    local_scope.DeclareInScope(
        node.GetToken().GetLexeme(),
        node.GetToken().GetLocation(),
        is_mutable,
        type_enum
    );
}
bool SemanticAnalysisVisitor::IsBeingDeclared(IdentifierNode& node) {
    ParseNode& parent = node.GetParent();
    return (&(parent.GetChild(0)) == &node)
        && (parent.GetNodeType() == ParseNodeType::DECLARATION_STATEMENT_NODE);
}

// Scoping
void SemanticAnalysisVisitor::CreateGlobalScope(ProgramNode& node) {
    node.SetScope(Scope::CreateGlobalScope());
}
void SemanticAnalysisVisitor::CreateSubscope(CodeBlockNode& node) {
    Scope& local_scope = node.GetLocalScope().value().get();
    node.SetScope(local_scope.CreateSubscope());
}
