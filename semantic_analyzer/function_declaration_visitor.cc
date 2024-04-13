#include <logging/punkt_logger.h>
#include <token/all_tokens.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <symbol_table/scope.h>

#include "function_declaration_visitor.h"


//--------------------------------------------------------------------------------------//
//                                    Non-leaf nodes                                    //
//--------------------------------------------------------------------------------------//
void FunctionDeclarationVisitor::VisitEnter(FunctionNode& node) {
    
}
void FunctionDeclarationVisitor::VisitLeave(FunctionNode& node) {
    
}

void FunctionDeclarationVisitor::VisitLeave(FunctionParameterNode& node) {
    TypeNode *type_node = node.GetTypeNode();
    if (!type_node) {
        PunktLogger::LogFatalInternalError("FunctionParameterNode::GetTypeNode returned null");
    }

    const Type& paremeter_type = *type_node->GetType();

    node.SetType(Type::CreateType(paremeter_type));

    IdentifierNode *identifier_node = node.GetIdentifierNode();
    if (!identifier_node) {
        PunktLogger::LogFatalInternalError("FunctionParameterNode::GetIdentifierNode "
                "returned null");
    }
    identifier_node->SetType(Type::CreateType(paremeter_type));
}

void FunctionDeclarationVisitor::VisitEnter(FunctionPrototypeNode& node) {
    
}
void FunctionDeclarationVisitor::VisitLeave(FunctionPrototypeNode& node) {
    
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
