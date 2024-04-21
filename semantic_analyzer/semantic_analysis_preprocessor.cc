#include "semantic_analysis_preprocessor.h"

#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <semantic_analyzer/types/base_type.h>
#include <semantic_analyzer/types/lambda_type.h>
#include <symbol_table/scope.h>

#include <algorithm>
#include <iterator>

//--------------------------------------------------------------------------------------//
//                                    Non-leaf nodes //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisPreprocessor::VisitLeave(FunctionDefinitionNode &node) {
  auto lambda_node = node.GetLambdaNode();
  if (!lambda_node) {
    PunktLogger::LogFatalInternalError(
        "FunctionDefinitionNode::GetLambdaNode returned null");
  }

  auto identifier_node = node.GetIdentifierNode();
  if (!identifier_node) {
    PunktLogger::LogFatalInternalError(
        "FunctionDefinitionNode::GetIdentifierNode returned null");
  }
  identifier_node->SetType(lambda_node->GetType()->CreateEquivalentType());

  DeclareFunction(*identifier_node, identifier_node->GetType());
}

void SemanticAnalysisPreprocessor::VisitLeave(LambdaNode &node) {
  std::vector<LambdaParameterNode *> parameter_nodes = node.GetParameterNodes();
  std::vector<Type *> parameter_types;
  parameter_types.reserve(parameter_nodes.size());
  std::transform(parameter_nodes.begin(), parameter_nodes.end(),
                 std::inserter(parameter_types, parameter_types.end()),
                 [](const auto &param_node) { return param_node->GetType(); });

  ParseNode *return_type_node = node.GetReturnTypeNode();

  node.SetType(LambdaType::CreateLambdaType(parameter_types,
                                            return_type_node->GetType()));
}

void SemanticAnalysisPreprocessor::VisitLeave(LambdaParameterNode &node) {
  ParseNode *type_node = node.GetTypeNode();
  Type *parameter_type = type_node->GetType();

  auto parameter_base_type = dynamic_cast<BaseType *>(parameter_type);
  if (parameter_base_type &&
      parameter_base_type->GetBaseTypeEnum() == BaseTypeEnum::VOID) {
    // Parameter types cannot be void.
    VoidParameterTypeError(*type_node);
    node.SetType(BaseType::CreateErrorType());
    return;
  }

  node.SetType(parameter_type->CreateEquivalentType());

  IdentifierNode *identifier_node = node.GetIdentifierNode();
  if (!identifier_node) {
    PunktLogger::LogFatalInternalError(
        "LambdaParameterNode::GetIdentifierNode returned null");
  }
  identifier_node->SetType(parameter_type->CreateEquivalentType());
}

void SemanticAnalysisPreprocessor::VisitLeave(LambdaTypeNode &node) {
  node.SetType(node.InferOwnType());
}

void SemanticAnalysisPreprocessor::VisitEnter(ProgramNode &node) {
  CreateGlobalScope(node);
}

//--------------------------------------------------------------------------------------//
//                                      Leaf nodes //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisPreprocessor::Visit(BaseTypeNode &node) {
  node.SetType(node.InferOwnType());
}

//--------------------------------------------------------------------------------------//
//                                       Scoping //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisPreprocessor::CreateGlobalScope(ParseNode &node) {
  node.SetScope(Scope::CreateGlobalScope());
}

//--------------------------------------------------------------------------------------//
//                                Miscellaneous helpers //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisPreprocessor::DeclareFunction(IdentifierNode &node,
                                                   Type *type) {
  Scope *local_scope = node.GetLocalScope();
  SymbolTableEntry *symbol_table_entry = local_scope->Declare(
      node.GetToken()->GetLexeme(), node.GetToken()->GetLocation(),
      /*is_mutable=*/false, type, SymbolType::LAMBDA);
  node.SetSymbolTableEntry(symbol_table_entry);
}

//--------------------------------------------------------------------------------------//
//                                   Error reporting //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisPreprocessor::VoidParameterTypeError(
    ParseNode &type_node) {
  std::string message = "parameter cannot have void type at " +
                        type_node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}
