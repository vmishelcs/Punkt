#include "semantic_analysis_visitor.h"

#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <semantic_analyzer/signatures/signatures.h>
#include <symbol_table/scope.h>
#include <token/keyword_token.h>
#include <token/operator_token.h>

#include "types/array_type.h"
#include "types/base_type.h"
#include "types/lambda_type.h"
#include "types/type.h"

static void DeallocOnNonArrayType(ParseNode &);

/******************************************************************************
 *                               Non-leaf nodes                               *
 ******************************************************************************/
void SemanticAnalysisVisitor::VisitLeave(CallStatementNode &node) {
  if (!node.GetLambdaInvocationNode()) {
    // Call statement must be followed by a lambda invocation.
    CallWithoutFunctionInvocationError(node);
    return;
  }
}

void SemanticAnalysisVisitor::VisitEnter(CodeBlockNode &node) {
  if (node.GetParent()->GetParseNodeType() == ParseNodeType::LAMBDA_NODE) {
    CreateProcedureScope(node);
  } else {
    CreateSubscope(node);
  }
}

void SemanticAnalysisVisitor::VisitLeave(DeallocStatementNode &node) {
  ParseNode *arg = node.GetChild(0);
  if (dynamic_cast<ArrayType *>(arg->GetType()) == nullptr) {
    DeallocOnNonArrayType(node);
    node.SetType(BaseType::CreateErrorType());
  }
}

// void SemanticAnalysisVisitor::VisitEnter(DeclarationStatementNode &node) {
//   bool is_mutable =
//       KeywordToken::IsTokenKeyword(node.GetToken(), {Keyword::VAR});

//   // Perform declaration here if the initializer is a lambda literal.
//   ParseNode *initializer = node.GetInitializer();
//   auto lambda_node = dynamic_cast<LambdaNode *>(initializer);
//   if (!lambda_node) {
//     return;
//   }

//   IdentifierNode *identifier = node.GetIdentifierNode();
//   if (!identifier) {
//     PunktLogger::LogFatalInternalError(
//         "VisitEnter(DeclarationStatementNode&): "
//         "incorrectly constructed declaration node.");
//   }

//   auto lambda_type = static_cast<LambdaType *>(lambda_node->GetType());

//   identifier->SetType(lambda_type->CreateEquivalentType());
//   DeclareInLocalScope(*identifier, is_mutable, identifier->GetType(),
//                       SymbolType::FUNCTION);
// }

void SemanticAnalysisVisitor::VisitLeave(DeclarationStatementNode &node) {
  bool is_mutable =
      KeywordToken::IsTokenKeyword(node.GetToken(), {Keyword::VAR});

  IdentifierNode *identifier = node.GetIdentifierNode();
  if (!identifier) {
    PunktLogger::LogFatalInternalError(
        "VisitLeave(DeclarationStatementNode&): "
        "incorrectly constructed declaration node.");
  }

  ParseNode *initializer = node.GetInitializer();
  Type *declaration_type = initializer->GetType();

  // Make sure we are not declaring variables with void type.
  auto b_declaration_type = dynamic_cast<BaseType *>(declaration_type);
  if (b_declaration_type &&
      b_declaration_type->IsEquivalentTo(BaseTypeEnum::VOID)) {
    DeclarationOfVarWithVoidTypeError(node);
    identifier->SetType(BaseType::CreateErrorType());
    return;
  }

  identifier->SetType(declaration_type->CreateEquivalentType());

  // Note the use of identifier-owned Type pointer.
  DeclareInLocalScope(*identifier, is_mutable, identifier->GetType(),
                      SymbolType::VARIABLE);
}

void SemanticAnalysisVisitor::VisitEnter(ForStatementNode &node) {
  CreateSubscope(node);
}
void SemanticAnalysisVisitor::VisitLeave(ForStatementNode &node) {
  // Make sure that the condition has boolean type.
  Type *condition_type = node.GetEndConditionNode()->GetType();
  BaseType *b_condition_type = dynamic_cast<BaseType *>(condition_type);

  if (!b_condition_type ||
      !b_condition_type->IsEquivalentTo(BaseTypeEnum::BOOLEAN)) {
    NonBooleanConditionError(node);
    node.SetType(BaseType::CreateErrorType());
    return;
  }
}

void SemanticAnalysisVisitor::VisitLeave(IfStatementNode &node) {
  // Make sure that the condition has boolean type.
  Type *condition_type = node.GetIfConditionNode()->GetType();
  BaseType *b_condition_type = dynamic_cast<BaseType *>(condition_type);

  if (!b_condition_type ||
      !b_condition_type->IsEquivalentTo(BaseTypeEnum::BOOLEAN)) {
    NonBooleanConditionError(node);
    node.SetType(BaseType::CreateErrorType());
    return;
  }
}

void SemanticAnalysisVisitor::VisitLeave(LambdaInvocationNode &node) {
  ParseNode *callee_node = node.GetCalleeNode();

  Type *type = callee_node->GetType();
  auto lambda_type = dynamic_cast<LambdaType *>(type);
  if (!lambda_type) {
    InvocationExpressionWithNonLambdaTypeError();
    node.SetType(BaseType::CreateErrorType());
    return;
  }

  std::vector<ParseNode *> arg_nodes = node.GetArgumentNodes();
  std::vector<Type *> arg_types;
  arg_types.reserve(arg_nodes.size());
  // Fill `arg_types` vector with corresponding types.
  std::transform(arg_nodes.begin(), arg_nodes.end(),
                 std::inserter(arg_types, arg_types.end()),
                 [](const auto &arg_node) { return arg_node->GetType(); });

  // Check that the lambda accepts the provided arguments.
  if (!lambda_type->AcceptsArgumentTypes(arg_types)) {
    LambdaDoesNotAcceptProvidedTypesError();
    node.SetType(BaseType::CreateErrorType());
    return;
  }

  // This node's type is the return type of the lambda.
  node.SetType(lambda_type->GetReturnType()->CreateEquivalentType());
}

void SemanticAnalysisVisitor::VisitEnter(LambdaNode &node) {
  CreateParameterScope(node);
}

void SemanticAnalysisVisitor::VisitLeave(LambdaParameterNode &node) {
  IdentifierNode *identifier_node = node.GetIdentifierNode();
  if (!identifier_node) {
    PunktLogger::LogFatalInternalError(
        "LambdaParameterNode::GetIdentifierNode returned null");
  }

  DeclareInLocalScope(*identifier_node, /*is_mutable=*/true,
                      identifier_node->GetType(), SymbolType::VARIABLE);
}

void SemanticAnalysisVisitor::VisitLeave(OperatorNode &node) {
  std::vector<Type *> child_types;
  child_types.reserve(node.NumChildren());

  for (auto child : node.GetChildren()) {
    Type *child_type = child->GetType();
    if (child_type->IsErrorType()) {
      node.SetType(BaseType::CreateErrorType());
      return;
    }
    child_types.push_back(child_type);
  }

  // Assignment semantic analysis is performed here.
  if (OperatorToken::IsTokenOperator(node.GetToken(), {Operator::ASSIGN})) {
    // Make sure left-hand side is targettable.
    if (auto id_node = dynamic_cast<IdentifierNode *>(node.GetChild(0))) {
      // Make sure variable is not `const`.
      if (!id_node->GetSymbolTableEntry()->is_mutable) {
        AssignmentToImmutableTargetError(*id_node);
        node.SetType(BaseType::CreateErrorType());
        return;
      }
    } else if (auto op_node = dynamic_cast<OperatorNode *>(node.GetChild(0))) {
      // Make sure the operator is array indexing.
      if (op_node->GetOperatorEnum() != Operator::ARRAY_IDX) {
        NonTargettableExpressionError(node);
        node.SetType(BaseType::CreateErrorType());
        return;
      }
    } else {
      // Any other operand is not targettable.
      NonTargettableExpressionError(node);
      node.SetType(BaseType::CreateErrorType());
      return;
    }
  }

  auto operator_token = static_cast<OperatorToken *>(node.GetToken());
  Signature *signature = signatures::AcceptingSignature(
      operator_token->GetOperatorEnum(), child_types);

  if (signature) {
    node.SetType(signature->GetOutputType()->CreateEquivalentType());
    node.SetCodegenFunction(signature->GetCodegenFunction());
  } else {
    InvalidOperandTypeError(node, child_types);
    node.SetType(BaseType::CreateErrorType());
  }
}

void SemanticAnalysisVisitor::VisitLeave(PrintStatementNode &node) {
  // Print only non-void type values.
  for (const auto &p_node : node.GetChildren()) {
    // Cannot print values of type void.
    auto base_type = dynamic_cast<BaseType *>(p_node->GetType());
    if (base_type && base_type->IsEquivalentTo(BaseTypeEnum::VOID)) {
      PrintingVoidTypeError(node);
      return;
    }
  }
}

void SemanticAnalysisVisitor::VisitLeave(ReturnStatementNode &node) {
  ParseNode *enclosing_function = node.GetEnclosingFunctionNode();
  if (!enclosing_function) {
    ReturnStatementOutsideOfFunctionError(node);
    return;
  }

  if (enclosing_function->GetParseNodeType() == ParseNodeType::MAIN_NODE) {
    // We do not allow returning values from main.
    if (node.NumChildren() > 0) {
      MainReturnStatementReturnsValueError(node);
    }
    return;
  }

  // Otherwise, the enclosing function must be a lambda.
  auto lambda = static_cast<LambdaNode *>(enclosing_function);

  Type *lambda_return_type = lambda->GetReturnTypeNode()->GetType();

  // If the enclosing lambda is declared as returning void, the return
  // statement must not return a value.
  auto lambda_return_b_type = dynamic_cast<BaseType *>(lambda_return_type);
  if (lambda_return_b_type &&
      lambda_return_b_type->IsEquivalentTo(BaseTypeEnum::VOID)) {
    if (node.NumChildren() > 0)
      ReturnStatementReturnsValueFromVoidLambdaError(node);
    return;
  }

  // Make sure this statement is return a value that is semantically equivalent
  // to the return type of the enclosing lambda.
  Type *return_value_type = node.GetReturnValueNode()->GetType();
  if (!lambda_return_type->IsEquivalentTo(return_value_type)) {
    IncompatibleReturnTypeError(node);
    return;
  }
}

void SemanticAnalysisVisitor::VisitLeave(WhileStatementNode &node) {
  // Make sure that the condition has boolean type.
  Type *condition_type = node.GetConditionNode()->GetType();
  BaseType *b_condition_type = dynamic_cast<BaseType *>(condition_type);

  if (!b_condition_type ||
      !b_condition_type->IsEquivalentTo(BaseTypeEnum::BOOLEAN)) {
    NonBooleanConditionError(node);
    node.SetType(BaseType::CreateErrorType());
    return;
  }
}

/******************************************************************************
 *                                 Leaf nodes                                 *
 ******************************************************************************/
void SemanticAnalysisVisitor::Visit(ErrorNode &node) {
  node.SetType(BaseType::CreateErrorType());
}
void SemanticAnalysisVisitor::Visit(IdentifierNode &node) {
  if (IsBeingDeclared(node) || IsParameterIdentifier(node)) {
    // If an identifier is being declared, its semantic analysis is handled
    // by either `VisitLeave(DeclarationStatementNode&)` or
    // `VisitLeave(FunctionDefinitionNode)`. If an identifier is part of a
    // function parameter, its semantic analysis is handled by
    // `VisitLeave(LambdaParameterNode&)`.
    return;
  }

  SymbolTableEntry *sym_table_entry = node.FindSymbolTableEntry();

  if (!sym_table_entry) {
    SymbolTable::UndefinedSymbolReference(node.GetToken()->GetLexeme(),
                                          node.GetToken()->GetLocation());
    node.SetType(BaseType::CreateErrorType());
    // Note the use of identifier-owned Type pointer.
    DeclareInLocalScope(node, false, node.GetType(), SymbolType::VARIABLE);
  } else {
    node.SetType(sym_table_entry->type->CreateEquivalentType());
    node.SetSymbolTableEntry(sym_table_entry);
  }
}
void SemanticAnalysisVisitor::Visit(BooleanLiteralNode &node) {
  node.SetType(BaseType::CreateBooleanType());
}
void SemanticAnalysisVisitor::Visit(CharacterLiteralNode &node) {
  node.SetType(BaseType::CreateCharacterType());
}
void SemanticAnalysisVisitor::Visit(IntegerLiteralNode &node) {
  node.SetType(BaseType::CreateIntegerType());
}
void SemanticAnalysisVisitor::Visit(StringLiteralNode &node) {
  node.SetType(BaseType::CreateStringType());
}

/******************************************************************************
 *                           Miscellaneous helpers                            *
 ******************************************************************************/
void SemanticAnalysisVisitor::DeclareInLocalScope(IdentifierNode &node,
                                                  bool is_mutable, Type *type,
                                                  SymbolType symbol_type) {
  Scope *local_scope = node.GetLocalScope();
  SymbolTableEntry *entry = local_scope->Declare(node.GetToken()->GetLexeme(),
                                                 node.GetToken()->GetLocation(),
                                                 is_mutable, type, symbol_type);
  node.SetSymbolTableEntry(entry);
}
bool SemanticAnalysisVisitor::IsBeingDeclared(IdentifierNode &node) {
  auto parent = node.GetParent();
  return (parent->GetChild(0) == &node) &&
         (parent->GetParseNodeType() ==
              ParseNodeType::DECLARATION_STATEMENT_NODE ||
          parent->GetParseNodeType() ==
              ParseNodeType::FUNCTION_DEFINITION_NODE);
}
bool SemanticAnalysisVisitor::IsParameterIdentifier(IdentifierNode &node) {
  return node.GetParent()->GetParseNodeType() ==
         ParseNodeType::LAMBDA_PARAMETER_NODE;
}

/******************************************************************************
 *                                  Scoping                                   *
 ******************************************************************************/
void SemanticAnalysisVisitor::CreateParameterScope(ParseNode &node) {
  Scope *local_scope = node.GetLocalScope();
  node.SetScope(local_scope->CreateParameterScope());
}
void SemanticAnalysisVisitor::CreateProcedureScope(ParseNode &node) {
  Scope *local_scope = node.GetLocalScope();
  node.SetScope(local_scope->CreateProcedureScope());
}
void SemanticAnalysisVisitor::CreateSubscope(ParseNode &node) {
  Scope *local_scope = node.GetLocalScope();
  node.SetScope(local_scope->CreateSubscope());
}

/******************************************************************************
 *                               Error handling                               *
 ******************************************************************************/
// TODO: These don't need to be methods.
void SemanticAnalysisVisitor::DeclarationOfVarWithVoidTypeError(
    DeclarationStatementNode &node) {
  std::string message = "variable declared with type void at " +
                        node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::InvalidOperandTypeError(
    OperatorNode &node, std::vector<Type *> &types) {
  std::string message =
      "operator \'" + node.GetToken()->GetLexeme() + "\' not defined for [";
  for (auto type : types) {
    message += type->ToString() + " ";
  }
  message.pop_back();
  message += "] at \n\t" + node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::NonBooleanConditionError(IfStatementNode &node) {
  std::string message = "if-statement at " +
                        node.GetToken()->GetLocation().ToString() +
                        " has non-boolean condition";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::NonBooleanConditionError(
    WhileStatementNode &node) {
  std::string message = "while-statement at " +
                        node.GetToken()->GetLocation().ToString() +
                        " has non-boolean condition";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::NonBooleanConditionError(ForStatementNode &node) {
  std::string message = "for-statement at " +
                        node.GetToken()->GetLocation().ToString() +
                        " has non-boolean condition";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::NonTargettableExpressionError(ParseNode &node) {
  std::string message =
      "non-targettable expression provided in assignment statement at \n\t" +
      node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::AssignmentToImmutableTargetError(
    ParseNode &node) {
  std::string message = "variable at " +
                        node.GetToken()->GetLocation().ToString() +
                        " is immutable";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::AssignmentTypeMismatchError(
    ParseNode &node, const Type &target_type, const Type &value_type) {
  std::string message = "cannot assign \'" + value_type.ToString() +
                        "\' value to a target of type \'" +
                        target_type.ToString() + "\'";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::PrintingVoidTypeError(PrintStatementNode &node) {
  std::string message = "cannot print void type value at " +
                        node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::InvocationExpressionWithNonLambdaTypeError() {
  std::string message = "invocation with non-lambda type";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::LambdaDoesNotAcceptProvidedTypesError() {
  std::string message = "invalid arguments for lambda invocation ";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::ReturnStatementOutsideOfFunctionError(
    ReturnStatementNode &node) {
  std::string message = "return statement outside of function at " +
                        node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::MainReturnStatementReturnsValueError(
    ReturnStatementNode &node) {
  std::string message = "cannot return value from main at " +
                        node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::ReturnStatementReturnsValueFromVoidLambdaError(
    ReturnStatementNode &node) {
  std::string message = "return statement at " +
                        node.GetToken()->GetLocation().ToString() +
                        " cannot return a value because the enclosing lambda "
                        "is declared to return void";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::IncompatibleReturnTypeError(
    ReturnStatementNode &node) {
  std::string message = "incompatible return type at " +
                        node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::CallWithoutFunctionInvocationError(
    CallStatementNode &node) {
  std::string message = "call statement without function invocation at " +
                        node.GetToken()->GetLocation().ToString();
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

static void DeallocOnNonArrayType(ParseNode &node) {
  std::string message = "dealloc used on non-array type";
  PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}
