#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <semantic_analyzer/signatures/signatures.h>
#include <symbol_table/scope.h>
#include <token/keyword_token.h>

#include "semantic_analysis_visitor.h"
#include "types/base_type.h"
#include "types/lambda_type.h"
#include "types/type.h"

//--------------------------------------------------------------------------------------//
//                                    Non-leaf nodes                                    //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::VisitLeave(AssignmentStatementNode& node) {
    auto target = node.GetChild(0);
    if (target->GetParseNodeType() == ParseNodeType::IDENTIFIER_NODE) {
        IdentifierNode *identifier_node = static_cast<IdentifierNode *>(target);

        // Make sure identifier is not classified with error type.
        if (identifier_node->GetType()->IsErrorType()) {
            node.SetType(BaseType::CreateErrorType());
            return;
        }

        // Make sure identifier is mutable.
        if (!identifier_node->GetSymbolTableEntry()->is_mutable) {
            AssignmentToImmutableTargetError(*identifier_node);
            node.SetType(BaseType::CreateErrorType());
            return;
        }

        auto new_value = node.GetChild(1);

        // Make sure we are assigning an equivalent type.
        if (!identifier_node->GetType()->IsEquivalentTo(new_value->GetType())) {
            AssignmentTypeMismatchError(*identifier_node, *identifier_node->GetType(),
                    *new_value->GetType());
            node.SetType(BaseType::CreateErrorType());
            return;
        }
    }
    else {
        NonTargettableExpressionError(*target);
        node.SetType(BaseType::CreateErrorType());
    }
}

void SemanticAnalysisVisitor::VisitEnter(CodeBlockNode& node) {
    if (node.GetParent()->GetParseNodeType() == ParseNodeType::LAMBDA_NODE) {
        CreateProcedureScope(node);
    }
    else {
        CreateSubscope(node);
    }
}

void SemanticAnalysisVisitor::VisitEnter(DeclarationStatementNode& node) {
    bool is_mutable = KeywordToken::IsTokenKeyword(node.GetToken(), {KeywordEnum::VAR});

    // Perform declaration here if the initializer is a lambda literal.
    ParseNode *initializer = node.GetInitializer();
    auto lambda_node = dynamic_cast<LambdaNode *>(initializer);
    if (!lambda_node) {
        return;
    }

    IdentifierNode *identifier = node.GetIdentifierNode();
    if (!identifier) {
        PunktLogger::LogFatalInternalError("VisitEnter(DeclarationStatementNode&): "
                "incorrectly constructed declaration node.");
    }

    auto lambda_type = dynamic_cast<LambdaType *>(lambda_node->GetType());
    if (!lambda_type) {
        PunktLogger::LogFatalInternalError("VisitEnter(DeclarationStatementNode&): "
                "LambdaNode has non-lambda type.");
    }

    identifier->SetType(lambda_type->CreateEquivalentType());
    DeclareInLocalScope(*identifier, is_mutable, identifier->GetType(), SymbolType::LAMBDA);
}

void SemanticAnalysisVisitor::VisitLeave(DeclarationStatementNode& node) {
    bool is_mutable = KeywordToken::IsTokenKeyword(node.GetToken(), {KeywordEnum::VAR});

    IdentifierNode *identifier = node.GetIdentifierNode();
    if (!identifier) {
        PunktLogger::LogFatalInternalError("VisitLeave(DeclarationStatementNode&): "
                "incorrectly constructed declaration node.");
    }
    
    ParseNode *initializer = node.GetInitializer();
    if (dynamic_cast<LambdaNode *>(initializer)) {
        // Declaration for variables that hold lambda literals is done in
        // VisitEnter(DeclarationStatementNode&).
        return;
    }

    Type *declaration_type = initializer->GetType();
    identifier->SetType(declaration_type->CreateEquivalentType());

    // Note the use of identifier-owned Type pointer.
    DeclareInLocalScope(*identifier, is_mutable, identifier->GetType(), SymbolType::VARIABLE);
}

void SemanticAnalysisVisitor::VisitEnter(ForStatementNode& node) {
    CreateSubscope(node);
}
void SemanticAnalysisVisitor::VisitLeave(ForStatementNode& node) {
    // Make sure that the condition has boolean type.
    Type *condition_type = node.GetChild(1)->GetType();
    BaseType *b_condition_type = dynamic_cast<BaseType *>(condition_type);
    
    if (!b_condition_type || !b_condition_type->IsEquivalentTo(BaseTypeEnum::BOOLEAN)) {
        NonBooleanConditionError(node);
        node.SetType(BaseType::CreateErrorType());
        return;
    }
}

void SemanticAnalysisVisitor::VisitLeave(IfStatementNode& node) {
    // Make sure that the condition has boolean type.
    Type *condition_type = node.GetChild(0)->GetType();
    BaseType *b_condition_type = dynamic_cast<BaseType *>(condition_type);

    if (!b_condition_type || !b_condition_type->IsEquivalentTo(BaseTypeEnum::BOOLEAN)) {
        NonBooleanConditionError(node);
        node.SetType(BaseType::CreateErrorType());
        return;
    }
}

void SemanticAnalysisVisitor::VisitLeave(LambdaInvocationNode& node) {
    ParseNode *callee_node = node.GetCalleeNode();

    Type *type = callee_node->GetType();
    auto lambda_type = dynamic_cast<LambdaType *>(type);
    if (!lambda_type) {
        InvocationExpressionWithNonLambdaTypeError(*callee_node);
        node.SetType(BaseType::CreateErrorType());
        return;
    }

    std::vector<ParseNode *> arg_nodes = node.GetArgumentNodes();
    std::vector<Type *> arg_types;
    arg_types.reserve(arg_nodes.size());
    // Fill `arg_types` vector with corresponding types.
    std::transform(arg_nodes.begin(), arg_nodes.end(), std::inserter(arg_types, arg_types.end()),
            [](const auto& arg_node) { return arg_node->GetType(); });
    
    // Check that the lambda accepts the provided arguments.
    if (!lambda_type->AcceptsArgumentTypes(arg_types)) {
        LambdaDoesNotAcceptProvidedTypesError(*callee_node);
        node.SetType(BaseType::CreateErrorType());
        return;
    }

    // This node's type is the return type of the lambda.
    node.SetType(lambda_type->GetReturnType()->CreateEquivalentType());
}

void SemanticAnalysisVisitor::VisitEnter(LambdaNode& node) {
    CreateParameterScope(node);
}

void SemanticAnalysisVisitor::VisitLeave(LambdaParameterNode& node) {
    IdentifierNode *identifier_node = node.GetIdentifierNode();
    if (!identifier_node) {
        PunktLogger::LogFatalInternalError(
                "LambdaParameterNode::GetIdentifierNode returned null");
    }

    DeclareInLocalScope(*identifier_node, /*is_mutable=*/true, identifier_node->GetType(),
            SymbolType::VARIABLE);
}

void SemanticAnalysisVisitor::VisitLeave(OperatorNode& node) {
    std::vector<Type *> child_types;
    for (auto child : node.GetChildren()) {
        Type *child_type = child->GetType();
        if (child_type->IsErrorType()) {
            node.SetType(BaseType::CreateErrorType());
            return;
        }
        child_types.push_back(child_type);
    }

    PunctuatorToken *punctuator_token = dynamic_cast<PunctuatorToken *>(node.GetToken());
    if (!punctuator_token) {
        node.SetType(BaseType::CreateErrorType());
        return;
    }

    auto signature = Signatures::AcceptingSignature(
        punctuator_token->GetPunctuatorEnum(),
        child_types
    );

    if (signature) {
        node.SetType(signature->GetOutputType()->CreateEquivalentType());
        node.SetCodeGenFunc(signature->GetCodeGenFunc());
    }
    else {
        InvalidOperandTypeError(node, child_types);
        node.SetType(BaseType::CreateErrorType());
    }
}

void SemanticAnalysisVisitor::VisitLeave(ReturnStatementNode& node) {
    // Make sure this statement is returning an expression that is semantically equivalent to the
    // lambda return type this this return statement is a part of.
    Type *ret_expr_type = node.GetChild(0)->GetType();

    LambdaNode *lambda_node = node.GetEnclosingLambdaNode();
    auto lambda_type = dynamic_cast<LambdaType *>(lambda_node->GetType());
    if (!lambda_type) {
        PunktLogger::LogFatalInternalError("LambdaNode has non-lambda type");
    }

    if (!lambda_type->GetReturnType()->IsEquivalentTo(ret_expr_type)) {
        IncompatibleReturnTypeError(node);
        node.SetType(BaseType::CreateErrorType());
        return;
    }
}

//--------------------------------------------------------------------------------------//
//                                      Leaf nodes                                      //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::Visit(ErrorNode& node) {
    node.SetType(BaseType::CreateErrorType());
}
void SemanticAnalysisVisitor::Visit(IdentifierNode& node) {
    if (IsBeingDeclared(node) || IsParameterIdentifier(node)) {
        // If an identifier is being declared, its semantic analysis is handled by either
        // `VisitLeave(DeclarationStatementNode&)` or `VisitLeave(FunctionDefinitionNode)`.If an
        // identifier is part of a function parameter, its semantic analysis is handled by 
        // `VisitLeave(LambdaParameterNode&)`.
        return;
    }

    SymbolTableEntry *sym_table_entry = node.FindSymbolTableEntry();

    if (!sym_table_entry) {
        SymbolTable::UndefinedSymbolReference(
            node.GetToken()->GetLexeme(),
            node.GetToken()->GetLocation()
        );
        node.SetType(BaseType::CreateErrorType());
        // Note the use of identifier-owned Type pointer.
        DeclareInLocalScope(node, false, node.GetType(), SymbolType::VARIABLE);
    }
    else {
        node.SetType(sym_table_entry->type->CreateEquivalentType());
        node.SetSymbolTableEntry(sym_table_entry);
    }
}
void SemanticAnalysisVisitor::Visit(BooleanLiteralNode& node) {
    node.SetType(BaseType::CreateBooleanType());
}
void SemanticAnalysisVisitor::Visit(CharacterLiteralNode& node) {
    node.SetType(BaseType::CreateCharacterType());
}
void SemanticAnalysisVisitor::Visit(IntegerLiteralNode& node) {
    node.SetType(BaseType::CreateIntegerType());
}
void SemanticAnalysisVisitor::Visit(StringLiteralNode& node) {
    node.SetType(BaseType::CreateStringType());
}
void SemanticAnalysisVisitor::Visit(TypeNode& node) {
    // Perform semantic analysis only on TypeNodes that are NOT a part of a parameter and DO NOT
    // specify a lambda return type. Semantic analysis of TypeNodes that denote parameter types or
    // return types is done in FunctionDefinitionVisitor::Visit(TypeNode&).
    if (node.DenotesParameterType() || node.DenotesReturnType()) {
        return;
    }

    node.InferOwnType();
}

//--------------------------------------------------------------------------------------//
//                                Miscellaneous helpers                                 //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::DeclareInLocalScope(IdentifierNode& node, bool is_mutable, Type *type,
        SymbolType symbol_type)
{
    Scope *local_scope = node.GetLocalScope();
    SymbolTableEntry *entry = local_scope->Declare(
        node.GetToken()->GetLexeme(),
        node.GetToken()->GetLocation(),
        is_mutable,
        type,
        symbol_type
    );
    node.SetSymbolTableEntry(entry);
}
bool SemanticAnalysisVisitor::IsBeingDeclared(IdentifierNode& node) {
    auto parent = node.GetParent();
    return (parent->GetChild(0) == &node)
        && (parent->GetParseNodeType() == ParseNodeType::DECLARATION_STATEMENT_NODE
            || parent->GetParseNodeType() == ParseNodeType::FUNCTION_DEFINITION_NODE);
}
bool SemanticAnalysisVisitor::IsParameterIdentifier(IdentifierNode& node) {
    return node.GetParent()->GetParseNodeType() == ParseNodeType::LAMBDA_PARAMETER_NODE;
}

//--------------------------------------------------------------------------------------//
//                                       Scoping                                        //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::CreateParameterScope(ParseNode& node) {
    Scope *local_scope = node.GetLocalScope();
    node.SetScope(local_scope->CreateParameterScope());
}
void SemanticAnalysisVisitor::CreateProcedureScope(ParseNode& node) {
    Scope *local_scope = node.GetLocalScope();
    node.SetScope(local_scope->CreateProcedureScope());
}
void SemanticAnalysisVisitor::CreateSubscope(ParseNode& node) {
    Scope *local_scope = node.GetLocalScope();
    node.SetScope(local_scope->CreateSubscope());
}

//--------------------------------------------------------------------------------------//
//                                    Error handling                                    //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::InvalidOperandTypeError(OperatorNode& node, std::vector<Type*>& types)
{
    std::string message = "operator \'" + node.GetToken()->GetLexeme() + "\' not defined for [";
    for (auto type : types) {
        message += type->ToString() + " ";
    }
    message.pop_back();
    message += "] at \n\t" + node.GetToken()->GetLocation().ToString();
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);    
}

void SemanticAnalysisVisitor::NonBooleanConditionError(IfStatementNode& node) {
    std::string message = "if-statement at " + node.GetToken()->GetLocation().ToString()
            + " has non-boolean condition.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::NonBooleanConditionError(ForStatementNode& node) {
    std::string message = "for-statement at " + node.GetToken()->GetLocation().ToString()
            + " has non-boolean condition.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::NonTargettableExpressionError(ParseNode& node) {
    std::string message = "non-targettable expression provided in assignment statement at \n\t"
        + node.GetToken()->GetLocation().ToString();
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::AssignmentToImmutableTargetError(ParseNode& node) {
    std::string message = "variable at " + node.GetToken()->GetLocation().ToString()
            + " is immutable.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::AssignmentTypeMismatchError(ParseNode& node, const Type& target_type,
            const Type& value_type) {
    std::string message = "cannot assign \'" + value_type.ToString()
            + "\' value to a target of type \'" + target_type.ToString() + "\'.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::InvocationExpressionWithNonLambdaTypeError(ParseNode& node) {
    std::string message = "expression at " + node.GetToken()->GetLocation().ToString()
            + " has non-lambda type.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::LambdaDoesNotAcceptProvidedTypesError(ParseNode& node) {
    std::string message = "invalid arguments for lambda invocation.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::IncompatibleReturnTypeError(ReturnStatementNode& node) {
    std::string message = "incompatible return type at "
            + node.GetToken()->GetLocation().ToString();
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}
