#include "semantic_analyzer.h"

std::unique_ptr<ParseNode> SemanticAnalyzer::Analyze(std::unique_ptr<ParseNode> ast) {
    SemanticAnalyzer semantic_analyzer(std::move(ast));
    return semantic_analyzer.BeginSemanticAnalysis();
}

SemanticAnalyzer::SemanticAnalyzer(std::unique_ptr<ParseNode> ast)
    : ast(std::move(ast))
{}

std::unique_ptr<ParseNode> SemanticAnalyzer::BeginSemanticAnalysis() {
    SemanticAnalysisVisitor semantic_analysis_visitor;
    ast->Accept(semantic_analysis_visitor);

    return std::move(ast);
}
