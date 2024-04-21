#ifndef SEMANTIC_ANALYSIS_PREPROCESSOR_H_
#define SEMANTIC_ANALYSIS_PREPROCESSOR_H_

#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_nodes/all_nodes.h>

class SemanticAnalysisPreprocessor : public DefaultParseNodeVisitor {
 public:
  // ---- Non-leaf nodes -------------------------------------------------------
  virtual void VisitLeave(FunctionDefinitionNode &node) override;

  virtual void VisitLeave(LambdaNode &node) override;

  virtual void VisitLeave(LambdaParameterNode &node) override;

  virtual void VisitLeave(LambdaTypeNode &node) override;

  virtual void VisitEnter(ProgramNode &node) override;

  // ---- Leaf nodes -----------------------------------------------------------
  virtual void Visit(BaseTypeNode &node) override;

 private:
  // ---- Scoping --------------------------------------------------------------
  void CreateGlobalScope(ParseNode &node);

  // ---- Miscellaneous helpers ------------------------------------------------
  void DeclareFunction(IdentifierNode &node, Type *type);

  // ---- Error reporting ------------------------------------------------------
  void VoidParameterTypeError(ParseNode &type_node);
};

#endif  // SEMANTIC_ANALYSIS_PREPROCESSOR_H_
