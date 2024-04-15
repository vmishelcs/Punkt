#include <algorithm>
#include <iterator>

#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <symbol_table/scope.h>
#include <semantic_analyzer/types/lambda_type.h>

#include "function_definition_visitor.h"


//--------------------------------------------------------------------------------------//
//                                    Non-leaf nodes                                    //
//--------------------------------------------------------------------------------------//
void FunctionDefinitionVisitor::VisitLeave(FunctionDefinitionNode& node) {
    auto lambda_node = node.GetLambdaNode();
    if (!lambda_node) {
        PunktLogger::LogFatalInternalError("FunctionDefinitionNode::GetLambdaNode returned null");
    }

    auto identifier_node = node.GetIdentifierNode();
    if (!identifier_node) {
        PunktLogger::LogFatalInternalError(
                "FunctionDefinitionNode::GetIdentifierNode returned null");
    }
    identifier_node->SetType(lambda_node->GetType()->CreateEquivalentType());

    DeclareFunction(*identifier_node, identifier_node->GetType());
}

void FunctionDefinitionVisitor::VisitLeave(LambdaNode& node) {
    auto parameter_nodes = node.GetParameterNodes();
    std::vector<Type *> parameter_types;
    parameter_types.reserve(parameter_nodes.size());
    std::transform(parameter_nodes.begin(), parameter_nodes.end(),
            std::inserter(parameter_types, parameter_types.end()),
            [](const auto& param_node) { return param_node->GetType(); });

    auto return_type_node = node.GetReturnTypeNode();

    node.SetType(LambdaType::CreateLambdaType(parameter_types, return_type_node->GetType()));
}

void FunctionDefinitionVisitor::VisitLeave(LambdaParameterNode& node) {
    TypeNode *type_node = node.GetTypeNode();
    if (!type_node) {
        PunktLogger::LogFatalInternalError("LambdaParameterNode::GetTypeNode returned null");
    }

    Type *parameter_type = type_node->GetType();

    node.SetType(parameter_type->CreateEquivalentType());

    IdentifierNode *identifier_node = node.GetIdentifierNode();
    if (!identifier_node) {
        PunktLogger::LogFatalInternalError("LambdaParameterNode::GetIdentifierNode "
                "returned null");
    }
    identifier_node->SetType(parameter_type->CreateEquivalentType());
}

void FunctionDefinitionVisitor::VisitEnter(ProgramNode& node) {
    CreateGlobalScope(node);
}

//--------------------------------------------------------------------------------------//
//                                      Leaf nodes                                      //
//--------------------------------------------------------------------------------------//
void FunctionDefinitionVisitor::Visit(TypeNode& node) {
    // Perform semantic analysis only on TypeNodes that are a part of a parameter or specify a
    // return type. Semantic analysis of TypeNodes that do not denote a parameter type or a return
    // type is done in SemanticAnalysisVisitor::Visit(TypeNode&).
    if (!node.DenotesParameterType() && !node.DenotesReturnType()) {
        return;
    }

    node.InferOwnType();
}

//--------------------------------------------------------------------------------------//
//                                       Scoping                                        //
//--------------------------------------------------------------------------------------//
void FunctionDefinitionVisitor::CreateGlobalScope(ParseNode& node) {
    node.SetScope(Scope::CreateGlobalScope());
}

//--------------------------------------------------------------------------------------//
//                                Miscellaneous helpers                                 //
//--------------------------------------------------------------------------------------//
void FunctionDefinitionVisitor::DeclareFunction(IdentifierNode& node, Type *type) {
    Scope *local_scope = node.GetLocalScope();
    local_scope->Declare(
        node.GetToken()->GetLexeme(),
        node.GetToken()->GetLocation(),
        /*is_mutable=*/false,
        type,
        SymbolType::FUNCTION
    );
}
