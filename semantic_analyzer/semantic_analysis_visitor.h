#ifndef SEMANTIC_ANALYSIS_VISITOR_H_
#define SEMANTIC_ANALYSIS_VISITOR_H_

#include <parse_node/parse_node_visitor.h>

class SemanticAnalysisVisitor : public DefaultParseNodeVisitor {
 public:
  // ---- Non-leaf nodes -------------------------------------------------------
  virtual void VisitLeave(AllocExpressionNode &node) override;

  virtual void VisitEnter(CodeBlockNode &node) override;

  virtual void VisitLeave(DeallocStatementNode &node) override;

  virtual void VisitLeave(DeclarationStatementNode &node) override;

  virtual void VisitEnter(ForStatementNode &node) override;
  virtual void VisitLeave(ForStatementNode &node) override;

  virtual void VisitLeave(LambdaInvocationNode &node) override;

  virtual void VisitEnter(LambdaNode &node) override;

  virtual void VisitLeave(LambdaParameterNode &node) override;

  virtual void VisitLeave(IfStatementNode &node) override;

  virtual void VisitLeave(OperatorNode &node) override;

  virtual void VisitLeave(PopulatedArrayExpressionNode &node) override;

  virtual void VisitLeave(PrintStatementNode &node) override;

  virtual void VisitLeave(ReturnStatementNode &node) override;

  virtual void VisitLeave(WhileStatementNode &node) override;

  // ---- Leaf nodes -----------------------------------------------------------
  virtual void Visit(ErrorNode &node) override;
  virtual void Visit(IdentifierNode &node) override;
  virtual void Visit(BooleanLiteralNode &node) override;
  virtual void Visit(CharacterLiteralNode &node) override;
  virtual void Visit(IntegerLiteralNode &node) override;
  virtual void Visit(StringLiteralNode &node) override;

 private:
  // ---- Miscellaneous helpers ------------------------------------------------
  void DeclareInLocalScope(IdentifierNode &node, bool is_mutable, Type *type,
                           SymbolType symbol_type);
  bool IsBeingDeclared(IdentifierNode &node);
  bool IsParameterIdentifier(IdentifierNode &node);

  // ---- Scoping --------------------------------------------------------------
  void CreateParameterScope(ParseNode &node);
  void CreateProcedureScope(ParseNode &node);
  void CreateSubscope(ParseNode &node);
};

#endif  // SEMANTIC_ANALYSIS_VISITOR_H_
