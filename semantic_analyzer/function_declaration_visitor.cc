#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <symbol_table/scope.h>
#include <semantic_analyzer/types/lambda_type.h>

#include "function_declaration_visitor.h"


//--------------------------------------------------------------------------------------//
//                                    Non-leaf nodes                                    //
//--------------------------------------------------------------------------------------//
void FunctionDeclarationVisitor::VisitEnter(FunctionNode& node) {
    
}
void FunctionDeclarationVisitor::VisitLeave(FunctionNode& node) {
    auto prototype_node = node.GetFunctionPrototypeNode();
    if (!prototype_node) {
        PunktLogger::LogFatalInternalError("FunctionNode::GetFunctionPrototypeNode returned null");
    }

    auto identifier_node = node.GetIdentifierNode();
    if (!identifier_node) {
        PunktLogger::LogFatalInternalError("FunctionNode::GetIdentifierNode returned null");
    }
    identifier_node->SetType(prototype_node->GetType()->CreateEquivalentType());

    DeclareFunction(*identifier_node, identifier_node->GetType());
}

void FunctionDeclarationVisitor::VisitLeave(FunctionParameterNode& node) {
    TypeNode *type_node = node.GetTypeNode();
    if (!type_node) {
        PunktLogger::LogFatalInternalError("FunctionParameterNode::GetTypeNode returned null");
    }

    Type *parameter_type = type_node->GetType();

    node.SetType(parameter_type->CreateEquivalentType());

    IdentifierNode *identifier_node = node.GetIdentifierNode();
    if (!identifier_node) {
        PunktLogger::LogFatalInternalError("FunctionParameterNode::GetIdentifierNode "
                "returned null");
    }
    identifier_node->SetType(parameter_type->CreateEquivalentType());
}

void FunctionDeclarationVisitor::VisitLeave(FunctionPrototypeNode& node) {
    auto parameter_types = node.GetParameterTypes();
    auto return_type = node.GetReturnType();
    node.SetType(LambdaType::CreateLambdaType(parameter_types, return_type));
}

void FunctionDeclarationVisitor::VisitEnter(ProgramNode& node) {
    CreateGlobalScope(node);
}

// ---- Leaf nodes -----------------------------------------------------------------------
void FunctionDeclarationVisitor::Visit(TypeNode& node) {
    // Perform semantic analysis only on type nodes that are a part of a parameter or specify a
    // return type.
    if (node.GetParent()->GetParseNodeType() != ParseNodeType::FUNCTION_PARAMETER_NODE
        && node.GetParent()->GetParseNodeType() != ParseNodeType::FUNCTION_PROTOTYPE_NODE) {
        return;
    }

    node.InferOwnType();
}

//--------------------------------------------------------------------------------------//
//                                       Scoping                                        //
//--------------------------------------------------------------------------------------//
void FunctionDeclarationVisitor::CreateGlobalScope(ParseNode& node) {
    node.SetScope(Scope::CreateGlobalScope());
}

//--------------------------------------------------------------------------------------//
//                                Miscellaneous helpers                                 //
//--------------------------------------------------------------------------------------//
void FunctionDeclarationVisitor::DeclareFunction(IdentifierNode& node, Type *type) {
    Scope *local_scope = node.GetLocalScope();
    local_scope->Declare(
        node.GetToken()->GetLexeme(),
        node.GetToken()->GetLocation(),
        /*is_mutable=*/true,
        type
    );
}
