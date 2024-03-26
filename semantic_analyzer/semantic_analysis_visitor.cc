#include "semantic_analysis_visitor.h"

#include <symbol_table/scope.h>
#include <token/keyword_token.h>
#include <semantic_analyzer/signatures/signatures.h>
#include <logging/punkt_logger.h>

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

    Type& declaration_type = initializer.GetType();

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
    std::vector<std::reference_wrapper<const Type>> child_types;
    for (ParseNode& child : node.GetChildren()) {
        Type& child_type = child.GetType();
        if (child_type.IsErrorType()) {
            node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
            return;
        }
        child_types.push_back(child_type);
    }

    PunctuatorToken& punctuator_token = dynamic_cast<PunctuatorToken&>(node.GetToken());
    auto signature_opt = Signatures::AcceptingSignature(
        punctuator_token.GetPunctuatorEnum(),
        child_types
    );

    if (signature_opt.has_value()) {
        const Signature& signature = signature_opt.value();
        node.SetType(std::make_unique<Type>(signature.GetOutputType()));
    }
    else {
        InvalidOperandTypeError(node, child_types);
        node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
    }
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
        auto symbol_data_opt = node.FindIdentifierSymbolData();

        if (!symbol_data_opt.has_value()) {
            SymbolTable::UndefinedSymbolReference(
                node.GetToken().GetLexeme(),
                node.GetToken().GetLocation()
            );
            node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
            Declare(node, false, TypeEnum::ERROR);
        }
        else {
            const SymbolData& symbol_data = symbol_data_opt.value();
            node.SetType(std::make_unique<Type>(symbol_data.type));
        }
    }
    // Other semantic analysis is handled by VisitLeave(DeclarationStatementNode&)
}
void SemanticAnalysisVisitor::Visit(IntegerLiteralNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::INTEGER));
}

// Miscellaneous helpers
void SemanticAnalysisVisitor::Declare(IdentifierNode& node, bool is_mutable, const Type& type) {
    Scope& local_scope = node.GetLocalScope().value().get();
    local_scope.DeclareInScope(
        node.GetToken().GetLexeme(),
        node.GetToken().GetLocation(),
        is_mutable,
        type
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

// Error reporting
void SemanticAnalysisVisitor::InvalidOperandTypeError(OperatorNode& node, std::vector<std::reference_wrapper<const Type>>& types) {
    auto& logger = PunktLogger::GetInstance();

    std::string message = "Operator \'" + node.GetToken().GetLexeme()
        + "\' not defined for ";
    message += "[";
    for (const auto& elem : types) {
        message += elem.get().ToString();
        if (&elem != &types.back()) {
            message += ", ";
        }
    }
    message += "] at\n";
    message += ("\t" + node.GetToken().GetLocation().ToString());

    logger.Log(LogType::SEMANTIC_ANALYZER, message);    
}
