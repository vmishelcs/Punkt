#include <logging/punkt_logger.h>
#include <parse_node/parse_node.h>
#include <parse_node/parse_nodes/all_nodes.h>
#include <semantic_analyzer/signatures/signatures.h>
#include <symbol_table/scope.h>
#include <token/keyword_token.h>

#include "function_declaration_visitor.h"
#include "semantic_analysis_visitor.h"
#include "types/base_type.h"
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
        if (!identifier_node->FindSymbolTableEntry().value().get().is_mutable) {
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
    if (node.GetParent()->GetParseNodeType() == ParseNodeType::FUNCTION_NODE) {
        CreateProcedureScope(node);
    }
    else {
        CreateSubscope(node);
    }
}

void SemanticAnalysisVisitor::VisitLeave(DeclarationStatementNode& node) {
    bool is_mutable = KeywordToken::IsTokenKeyword(node.GetToken(), {KeywordEnum::VAR});

    auto identifier = dynamic_cast<IdentifierNode *>(node.GetChild(0));
    auto initializer = node.GetChild(1);

    Type *declaration_type = initializer->GetType();
    identifier->SetType(declaration_type->CreateEquivalentType());

    // Note the use of identifier-owned Type pointer.
    DeclareInLocalScope(*identifier, is_mutable, identifier->GetType());
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

void SemanticAnalysisVisitor::VisitEnter(FunctionNode& node) {
    // Declare the function identifier BEFORE entering the parameter scope; that is, first declare
    // the function identifier (in FunctionDeclarationVisitor), then create the parameter scope.
    CreateParameterScope(node);
}
void SemanticAnalysisVisitor::VisitLeave(FunctionNode& node) {

}

void SemanticAnalysisVisitor::VisitLeave(FunctionParameterNode& node) {
    IdentifierNode *identifier_node = node.GetIdentifierNode();
    if (!identifier_node) {
        PunktLogger::LogFatalInternalError(
                "FunctionParameterNode::GetIdentifierNode returned null");
    }

    DeclareInLocalScope(*identifier_node, /*is_mutable=*/true, identifier_node->GetType());
}

void SemanticAnalysisVisitor::VisitEnter(FunctionPrototypeNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(FunctionPrototypeNode& node) {

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

void SemanticAnalysisVisitor::VisitEnter(ProgramNode& node) {
    FunctionDeclarationVisitor function_declaration_visitor;
    node.Accept(function_declaration_visitor);
}

void SemanticAnalysisVisitor::VisitEnter(ReturnStatementNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(ReturnStatementNode& node) {

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
        // `VisitLeave(DeclarationStatementNode&)` or `VisitLeave(FunctionNode)`. If an identifier
        // is part of a function parameter, its semantic analysis is handled by 
        // `VisitLeave(FunctionParameterNode&)`.
        return;
    }

    auto symbol_table_entry_opt = node.FindSymbolTableEntry();

    if (!symbol_table_entry_opt.has_value()) {
        SymbolTable::UndefinedSymbolReference(
            node.GetToken()->GetLexeme(),
            node.GetToken()->GetLocation()
        );
        node.SetType(BaseType::CreateErrorType());
        // Note the use of identifier-owned Type pointer.
        DeclareInLocalScope(node, false, node.GetType());
    }
    else {
        const SymbolTableEntry& symbol_table_entry = symbol_table_entry_opt.value();
        node.SetType(symbol_table_entry.type->CreateEquivalentType());
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
    // Perform semantic analysis only on type nodes that are NOT a part of a parameter and DO NOT
    // specify a function return type.
    if (node.GetParent()->GetParseNodeType() == ParseNodeType::FUNCTION_PARAMETER_NODE
        || node.GetParent()->GetParseNodeType() == ParseNodeType::FUNCTION_PROTOTYPE_NODE) {
        return;
    }

    node.InferOwnType();
}

//--------------------------------------------------------------------------------------//
//                                Miscellaneous helpers                                 //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::DeclareInLocalScope(IdentifierNode& node, bool is_mutable, Type *type)
{
    Scope *local_scope = node.GetLocalScope();
    local_scope->Declare(
        node.GetToken()->GetLexeme(),
        node.GetToken()->GetLocation(),
        is_mutable,
        type
    );
}
bool SemanticAnalysisVisitor::IsBeingDeclared(IdentifierNode& node) {
    auto parent = node.GetParent();
    return (parent->GetChild(0) == &node)
        && (parent->GetParseNodeType() == ParseNodeType::DECLARATION_STATEMENT_NODE
            || parent->GetParseNodeType() == ParseNodeType::FUNCTION_NODE);
}
bool SemanticAnalysisVisitor::IsParameterIdentifier(IdentifierNode& node) {
    return node.GetParent()->GetParseNodeType() == ParseNodeType::FUNCTION_PARAMETER_NODE;
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
