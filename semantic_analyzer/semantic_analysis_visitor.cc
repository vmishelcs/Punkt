#include <symbol_table/scope.h>
#include <token/keyword_token.h>
#include <semantic_analyzer/signatures/signatures.h>
#include <logging/punkt_logger.h>

#include "semantic_analysis_visitor.h"
#include "type.h"

//--------------------------------------------------------------------------------------//
//                                    Non-leaf nodes                                    //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::VisitEnter(AssignmentStatementNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::VisitLeave(AssignmentStatementNode& node) {
    auto target = node.GetChild(0);
    if (target->GetParseNodeType() == ParseNodeType::IDENTIFIER_NODE) {
        IdentifierNode *identifier_node = static_cast<IdentifierNode *>(target);

        // Make sure identifier is not classified with error type.
        if (identifier_node->GetType()->EquivalentTo(TypeEnum::ERROR)) {
            node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
            return;
        }

        // Make sure identifier is mutable.
        if (!identifier_node->FindSymbolTableEntry().value().get().is_mutable) {
            AssignmentToImmutableTargetError(*identifier_node);
            node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
            return;
        }

        auto new_value = node.GetChild(1);

        // Make sure we are assigning an equivalent type.
        if (!identifier_node->GetType()->EquivalentTo(*new_value->GetType())) {
            AssignmentTypeMismatchError(*identifier_node, *identifier_node->GetType(),
                    *new_value->GetType());
            node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
            return;
        }
    }
    else {
        NonTargettableExpressionError(*target);
        node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
    }
}

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

    IdentifierNode *identifier = dynamic_cast<IdentifierNode *>(node.GetChild(0));
    auto initializer = node.GetChild(1);

    Type *declaration_type = initializer->GetType();

    identifier->SetType(std::make_unique<Type>(*declaration_type));

    // Note the use of identifier-owned Type pointer.
    DeclareInLocalScope(*identifier, is_mutable, identifier->GetType());
}

void SemanticAnalysisVisitor::VisitEnter(ForStatementNode& node) {
    CreateSubscope(node);
}
void SemanticAnalysisVisitor::VisitLeave(ForStatementNode& node) {
    // Make sure that the condition has boolean type.
    if ( !(node.GetChild(1)->GetType()->EquivalentTo(TypeEnum::BOOLEAN)) ) {
        NonBooleanConditionError(node);
        node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
    }
}

void SemanticAnalysisVisitor::VisitEnter(FunctionNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(FunctionNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(FunctionParameterNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::VisitLeave(FunctionParameterNode& node) {
    // Do nothing
}

void SemanticAnalysisVisitor::VisitEnter(FunctionPrototypeNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(FunctionPrototypeNode& node) {

}

void SemanticAnalysisVisitor::VisitEnter(IfStatementNode& node) {
}
void SemanticAnalysisVisitor::VisitLeave(IfStatementNode& node) {
    // We make sure that the condition has boolean type.
    if ( !(node.GetChild(0)->GetType()->EquivalentTo(TypeEnum::BOOLEAN)) ) {
        NonBooleanConditionError(node);
        node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
    }
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
    std::vector<Type *> child_types;
    for (auto child : node.GetChildren()) {
        Type *child_type = child->GetType();
        if (child_type->IsErrorType()) {
            node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
            return;
        }
        child_types.push_back(child_type);
    }

    PunctuatorToken& punctuator_token = dynamic_cast<PunctuatorToken&>(node.GetToken());
    auto signature = Signatures::AcceptingSignature(
        punctuator_token.GetPunctuatorEnum(),
        child_types
    );

    if (signature) {
        node.SetType(std::make_unique<Type>(signature->GetOutputType()));
        node.SetCodeGenFunc(signature->GetCodeGenFunc());
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

void SemanticAnalysisVisitor::VisitEnter(ReturnStatementNode& node) {

}
void SemanticAnalysisVisitor::VisitLeave(ReturnStatementNode& node) {

}

//--------------------------------------------------------------------------------------//
//                                      Leaf nodes                                      //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::Visit(ErrorNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
}
void SemanticAnalysisVisitor::Visit(NopNode& node) {
    // Do nothing
}
void SemanticAnalysisVisitor::Visit(IdentifierNode& node) {
    if (!IsBeingDeclared(node)) {
        auto symbol_table_entry_opt = node.FindSymbolTableEntry();

        if (!symbol_table_entry_opt.has_value()) {
            SymbolTable::UndefinedSymbolReference(
                node.GetToken().GetLexeme(),
                node.GetToken().GetLocation()
            );
            node.SetType(std::make_unique<Type>(TypeEnum::ERROR));
            // Note the use of identifier-owned Type pointer.
            DeclareInLocalScope(node, false, node.GetType());
        }
        else {
            const SymbolTableEntry& symbol_table_entry = symbol_table_entry_opt.value();
            node.SetType(std::make_unique<Type>(*symbol_table_entry.type));
        }
    }
}
void SemanticAnalysisVisitor::Visit(BooleanLiteralNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::BOOLEAN));
}
void SemanticAnalysisVisitor::Visit(CharacterLiteralNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::CHARACTER));
}
void SemanticAnalysisVisitor::Visit(IntegerLiteralNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::INTEGER));
}
void SemanticAnalysisVisitor::Visit(StringLiteralNode& node) {
    node.SetType(std::make_unique<Type>(TypeEnum::STRING));
}
void SemanticAnalysisVisitor::Visit(TypeNode& node) {
    
}

//--------------------------------------------------------------------------------------//
//                                Miscellaneous helpers                                 //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::DeclareInLocalScope(IdentifierNode& node, bool is_mutable, Type *type) {
    Scope *local_scope = node.GetLocalScope();
    local_scope->Declare(
        node.GetToken().GetLexeme(),
        node.GetToken().GetLocation(),
        is_mutable,
        type
    );
}
bool SemanticAnalysisVisitor::IsBeingDeclared(IdentifierNode& node) {
    auto parent = node.GetParent();
    return (parent->GetChild(0) == &node)
        && (parent->GetParseNodeType() == ParseNodeType::DECLARATION_STATEMENT_NODE);
}

//--------------------------------------------------------------------------------------//
//                                       Scoping                                        //
//--------------------------------------------------------------------------------------//
void SemanticAnalysisVisitor::CreateGlobalScope(ProgramNode& node) {
    node.SetScope(Scope::CreateGlobalScope());
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
    std::string message = "operator \'" + node.GetToken().GetLexeme() + "\' not defined for [";
    for (auto type : types) {
        message += type->ToString() + " ";
    }
    message.pop_back();
    message += "] at \n\t" + node.GetToken().GetLocation().ToString();
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);    
}

void SemanticAnalysisVisitor::NonBooleanConditionError(IfStatementNode& node) {
    std::string message = "if-statement at " + node.GetToken().GetLocation().ToString()
            + " has non-boolean condition.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::NonBooleanConditionError(ForStatementNode& node) {
    std::string message = "for-statement at " + node.GetToken().GetLocation().ToString()
            + " has non-boolean condition.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::NonTargettableExpressionError(ParseNode& node) {
    std::string message = "non-targettable expression provided in assignment statement at \n\t"
        + node.GetToken().GetLocation().ToString();
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::AssignmentToImmutableTargetError(ParseNode& node) {
    std::string message = "variable at " + node.GetToken().GetLocation().ToString()
            + " is immutable.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}

void SemanticAnalysisVisitor::AssignmentTypeMismatchError(ParseNode& node, const Type& target_type,
            const Type& value_type) {
    std::string message = "cannot assign \'" + value_type.ToString()
            + "\' value to a target of type \'" + target_type.ToString() + "\'.";
    PunktLogger::Log(LogType::SEMANTIC_ANALYZER, message);
}
