#include "semantic_analysis_visitor.h"

#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <scanner/operator.h>
#include <semantic_analyzer/signatures/signatures.h>
#include <symbol_table/scope.h>
#include <token/keyword_token.h>
#include <token/operator_token.h>

#include "types/array_type.h"
#include "types/base_type.h"
#include "types/lambda_type.h"
#include "types/type.h"

static void DeclarationOfVarWithVoidTypeError(ParseNode &);
static void UndefinedSymbolReferenceError(ParseNode &, const std::string &);
static void NonTargettableExpressionError(ParseNode &);
static void AssignmentToImmutableTargetError(ParseNode &);
static void AssignmentTypeMismatchError(ParseNode &, const Type &,
                                        const Type &);
static void InvalidOperandTypeError(ParseNode &, Operator,
                                    std::vector<Type *> &);
static void PrintingVoidTypeError(ParseNode &);
static void InvocationExpressionWithNonLambdaTypeError(ParseNode &);
static void LambdaDoesNotAcceptProvidedTypesError(ParseNode &);
static void ReturnStatementOutsideOfFunctionError(ParseNode &);
static void MainReturnStatementReturnsValueError(ParseNode &);
static void ReturningValueFromVoidLambdaError(ParseNode &);
static void IncompatibleReturnTypeError(ParseNode &, const Type &,
                                        const Type &);
static void NonVoidLambdaDoesNotReturnValue(ParseNode &);
static void NonArrayTypeInAllocExpressionError(ParseNode &);
static void VoidArraySubtypeError(ParseNode &);
static void NonIntegerAllocSizeOperand(ParseNode &);
static void PopulatedArrayTypeMismatchError(ParseNode &);
static void DeallocOnNonArrayType(ParseNode &);
static void NonBooleanConditionError(ParseNode &);

//===----------------------------------------------------------------------===//
// Non-leaf nodes
//===----------------------------------------------------------------------===//
void SemanticAnalysisVisitor::VisitLeave(AllocExpressionNode &node) {
  // Make sure an array type is specified.
  ArrayType *array_type =
      dynamic_cast<ArrayType *>(node.GetChild(0)->GetType());
  if (!array_type) {
    NonArrayTypeInAllocExpressionError(node);
    return;
  }

  // Make sure the array does not have void subtype.
  BaseType *base_subtype = dynamic_cast<BaseType *>(array_type->GetSubtype());
  if (base_subtype && base_subtype->IsEquivalentTo(BaseTypeEnum::VOID)) {
    VoidArraySubtypeError(node);
    return;
  }

  // Make sure the second operand is of integer type.
  BaseType *size_type = dynamic_cast<BaseType *>(node.GetChild(1)->GetType());
  if (!size_type || !size_type->IsEquivalentTo(BaseTypeEnum::INTEGER)) {
    NonIntegerAllocSizeOperand(node);
    return;
  }

  node.SetType(array_type->CreateEquivalentType());
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
  }
}

void SemanticAnalysisVisitor::VisitLeave(DeclarationStatementNode &node) {
  bool is_mutable =
      KeywordToken::IsTokenKeyword(node.GetToken(), {Keyword::VAR});

  IdentifierNode *identifier = node.GetIdentifierNode();

  ParseNode *initializer = node.GetInitializer();
  Type *declaration_type = initializer->GetType();

  // Make sure we are not declaring variables with void type.
  auto b_declaration_type = dynamic_cast<BaseType *>(declaration_type);
  if (b_declaration_type &&
      b_declaration_type->IsEquivalentTo(BaseTypeEnum::VOID)) {
    DeclarationOfVarWithVoidTypeError(node);
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
    return;
  }
}

void SemanticAnalysisVisitor::VisitLeave(LambdaInvocationNode &node) {
  ParseNode *callee_node = node.GetCalleeNode();

  Type *type = callee_node->GetType();
  auto lambda_type = dynamic_cast<LambdaType *>(type);
  if (!lambda_type) {
    InvocationExpressionWithNonLambdaTypeError(node);
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
    LambdaDoesNotAcceptProvidedTypesError(node);
    return;
  }

  // This node's type is the return type of the lambda.
  node.SetType(lambda_type->GetReturnType()->CreateEquivalentType());
}

void SemanticAnalysisVisitor::VisitEnter(LambdaNode &node) {
  CreateParameterScope(node);
}
void SemanticAnalysisVisitor::VisitLeave(LambdaNode &node) {
  LambdaType *lambda_type = static_cast<LambdaType *>(node.GetType());
  BaseType *ret_type = dynamic_cast<BaseType *>(lambda_type->GetReturnType());

  CodeBlockNode *lambda_body = node.GetLambdaBodyNode();
  // If this is a non-void lambda, make sure all control paths return a value.
  if (!ret_type->IsEquivalentTo(BaseTypeEnum::VOID) &&
      !lambda_body->DoAllControlPathsReturn()) {
    NonVoidLambdaDoesNotReturnValue(node);
    return;
  }
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
  if (node.GetOperatorEnum() == Operator::ASSIGN) {
    // Make sure left-hand side is targettable.
    if (auto id_node = dynamic_cast<IdentifierNode *>(node.GetChild(0))) {
      // Make sure variable is not `const`.
      if (!id_node->GetSymbolTableEntry()->is_mutable) {
        AssignmentToImmutableTargetError(*id_node);
        return;
      }
    } else if (auto op_node = dynamic_cast<OperatorNode *>(node.GetChild(0))) {
      // Make sure the operator is array indexing.
      if (op_node->GetOperatorEnum() != Operator::ARRAY_IDX) {
        NonTargettableExpressionError(node);
        return;
      }
    } else {
      // Any other operand is not targettable.
      NonTargettableExpressionError(node);
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
    if (node.GetOperatorEnum() == Operator::ASSIGN) {
      AssignmentTypeMismatchError(node, *child_types[0], *child_types[1]);
    } else {
      InvalidOperandTypeError(node, node.GetOperatorEnum(), child_types);
    }
  }
}

void SemanticAnalysisVisitor::VisitLeave(PopulatedArrayExpressionNode &node) {
  if (node.NumChildren() == 0) {
    return;
  }

  Type *subtype = node.GetChild(0)->GetType();
  for (auto child : node.GetChildren()) {
    if (!subtype->IsEquivalentTo(child->GetType())) {
      PopulatedArrayTypeMismatchError(node);
      return;
    }
  }

  node.SetType(ArrayType::CreateArrayType(subtype));
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
    if (node.NumChildren() > 0) ReturningValueFromVoidLambdaError(node);
    return;
  }

  // Make sure this statement is return a value that is semantically equivalent
  // to the return type of the enclosing lambda.
  Type *return_value_type = node.GetReturnValueNode()->GetType();
  if (!lambda_return_type->IsEquivalentTo(return_value_type)) {
    IncompatibleReturnTypeError(node, *return_value_type, *lambda_return_type);
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
    return;
  }
}

//===----------------------------------------------------------------------===//
// Leaf nodes
//===----------------------------------------------------------------------===//
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
    UndefinedSymbolReferenceError(node, node.GetName());
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

//===----------------------------------------------------------------------===//
// Miscellaneous helpers
//===----------------------------------------------------------------------===//
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

//===----------------------------------------------------------------------===//
// Scoping
//===----------------------------------------------------------------------===//
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

//===----------------------------------------------------------------------===//
// Error handling
//===----------------------------------------------------------------------===//
void DeclarationOfVarWithVoidTypeError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "declaration initializer has void type");
  node.SetType(BaseType::CreateErrorType());
}

void UndefinedSymbolReferenceError(ParseNode &node, const std::string &symbol) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "undefined symbol reference \'" + symbol + "\'");
  node.SetType(BaseType::CreateErrorType());
}

void InvalidOperandTypeError(ParseNode &node, Operator op,
                             std::vector<Type *> &types) {
  std::string message = "operator \'" + operator_utils::GetOperatorLexeme(op) +
                        "\' not defined for types [";
  for (const auto type : types) {
    message += type->ToString();
    message += ", ";
  }

  if (types.size() > 0) {
    // Pop the extra ", ".
    message.pop_back();
    message.pop_back();
  }

  message += "]";
  PunktLogger::LogCompileError(node.GetTextLocation(), message);

  node.SetType(BaseType::CreateErrorType());
}

void NonBooleanConditionError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "if-statement has non-boolean type");
  node.SetType(BaseType::CreateErrorType());
}

void NonTargettableExpressionError(ParseNode &node) {
  PunktLogger::LogCompileError(
      node.GetTextLocation(),
      "non-targettable expression in assignment expression");
  node.SetType(BaseType::CreateErrorType());
}

void AssignmentToImmutableTargetError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "assignment to immutable variable");
  node.SetType(BaseType::CreateErrorType());
}

void AssignmentTypeMismatchError(ParseNode &node, const Type &target_type,
                                 const Type &value_type) {
  PunktLogger::LogCompileError(
      node.GetTextLocation(),
      "cannot assign value of type \'" + value_type.ToString() +
          "\' to a target of type \'" + target_type.ToString() + "\'");
  node.SetType(BaseType::CreateErrorType());
}

void PrintingVoidTypeError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "cannot print void type");
  node.SetType(BaseType::CreateErrorType());
}

void InvocationExpressionWithNonLambdaTypeError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "invocation expression with non-lambda type");
  node.SetType(BaseType::CreateErrorType());
}

void LambdaDoesNotAcceptProvidedTypesError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "lambda does not accept provided types");
  node.SetType(BaseType::CreateErrorType());
}

void ReturnStatementOutsideOfFunctionError(ParseNode &node) {
  PunktLogger::LogCompileError(
      node.GetTextLocation(),
      "return statement outside of any lambda or function");
  node.SetType(BaseType::CreateErrorType());
}

void MainReturnStatementReturnsValueError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "cannot return a value form main");
  node.SetType(BaseType::CreateErrorType());
}

void ReturningValueFromVoidLambdaError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "cannot return a value from void lambda");
  node.SetType(BaseType::CreateErrorType());
}

void IncompatibleReturnTypeError(ParseNode &node, const Type &return_type,
                                 const Type &lambda_return_type) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "returning \'" + return_type.ToString() +
                                   "\' from lambda whose return type is \'" +
                                   lambda_return_type.ToString() + "\'");
  node.SetType(BaseType::CreateErrorType());
}

void NonVoidLambdaDoesNotReturnValue(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "non-void function does not return a value");
  node.SetType(BaseType::CreateErrorType());
}

void NonArrayTypeInAllocExpressionError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "alloc expression must specify an array type");
  node.SetType(BaseType::CreateErrorType());
}

void VoidArraySubtypeError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "array type cannot have void subtype");
  node.SetType(BaseType::CreateErrorType());
}

void NonIntegerAllocSizeOperand(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "alloc expression must have integral size type");
  node.SetType(BaseType::CreateErrorType());
}

void PopulatedArrayTypeMismatchError(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "populated array expression must have the same "
                               "type for each array value");
  node.SetType(BaseType::CreateErrorType());
}

void DeallocOnNonArrayType(ParseNode &node) {
  PunktLogger::LogCompileError(node.GetTextLocation(),
                               "dealloc used on non-array type");
  node.SetType(BaseType::CreateErrorType());
}
