#ifndef PARSE_NODE_VISITOR_H_
#define PARSE_NODE_VISITOR_H_

#include "parse_nodes/all_nodes.h"

class ParseNodeVisitor {
 public:
  // ---- Non-leaf nodes
  // -------------------------------------------------------------------
  virtual void VisitEnter(AllocExpressionNode &node) = 0;
  virtual void VisitLeave(AllocExpressionNode &node) = 0;

  virtual void VisitEnter(ArrayTypeNode &node) = 0;
  virtual void VisitLeave(ArrayTypeNode &node) = 0;

  virtual void VisitEnter(CallStatementNode &node) = 0;
  virtual void VisitLeave(CallStatementNode &node) = 0;

  virtual void VisitEnter(CodeBlockNode &node) = 0;
  virtual void VisitLeave(CodeBlockNode &node) = 0;

  virtual void VisitEnter(DeallocStatementNode &node) = 0;
  virtual void VisitLeave(DeallocStatementNode &node) = 0;

  virtual void VisitEnter(DeclarationStatementNode &node) = 0;
  virtual void VisitLeave(DeclarationStatementNode &node) = 0;

  virtual void VisitEnter(ExpressionStatementNode &node) = 0;
  virtual void VisitLeave(ExpressionStatementNode &node) = 0;

  virtual void VisitEnter(ForStatementNode &node) = 0;
  virtual void VisitLeave(ForStatementNode &node) = 0;

  virtual void VisitEnter(FunctionDefinitionNode &node) = 0;
  virtual void VisitLeave(FunctionDefinitionNode &node) = 0;

  virtual void VisitEnter(IfStatementNode &node) = 0;
  virtual void VisitLeave(IfStatementNode &node) = 0;

  virtual void VisitEnter(LambdaInvocationNode &node) = 0;
  virtual void VisitLeave(LambdaInvocationNode &node) = 0;

  virtual void VisitEnter(LambdaNode &node) = 0;
  virtual void VisitLeave(LambdaNode &node) = 0;

  virtual void VisitEnter(LambdaParameterNode &node) = 0;
  virtual void VisitLeave(LambdaParameterNode &node) = 0;

  virtual void VisitEnter(LambdaTypeNode &node) = 0;
  virtual void VisitLeave(LambdaTypeNode &node) = 0;

  virtual void VisitEnter(MainNode &node) = 0;
  virtual void VisitLeave(MainNode &node) = 0;

  virtual void VisitEnter(OperatorNode &node) = 0;
  virtual void VisitLeave(OperatorNode &node) = 0;

  virtual void VisitEnter(PopulatedArrayExpressionNode &node) = 0;
  virtual void VisitLeave(PopulatedArrayExpressionNode &node) = 0;

  virtual void VisitEnter(PrintStatementNode &node) = 0;
  virtual void VisitLeave(PrintStatementNode &node) = 0;

  virtual void VisitEnter(ProgramNode &node) = 0;
  virtual void VisitLeave(ProgramNode &node) = 0;

  virtual void VisitEnter(ReturnStatementNode &node) = 0;
  virtual void VisitLeave(ReturnStatementNode &node) = 0;

  virtual void VisitEnter(WhileStatementNode &node) = 0;
  virtual void VisitLeave(WhileStatementNode &node) = 0;

  // ---- Leaf nodes
  // -----------------------------------------------------------------------
  virtual void Visit(ErrorNode &node) = 0;
  virtual void Visit(NopNode &node) = 0;
  virtual void Visit(IdentifierNode &node) = 0;
  virtual void Visit(BooleanLiteralNode &node) = 0;
  virtual void Visit(CharacterLiteralNode &node) = 0;
  virtual void Visit(IntegerLiteralNode &node) = 0;
  virtual void Visit(StringLiteralNode &node) = 0;
  virtual void Visit(BaseTypeNode &node) = 0;
};

/// @brief A default implementation of the ParseNodeVisitor interface. Each
/// `VisitEnter`, `VisitLeave`, and `Visit` method performs a default "do
/// nothing" visit. The purpose of this class is to make sure that classes that
/// want to inherit the ParseNodeVisitor interface are not forced to have an
/// implementation for each `Visit`* method.
class DefaultParseNodeVisitor : public ParseNodeVisitor {
 public:
  // ---- Non-leaf nodes
  // -------------------------------------------------------------------
  virtual void VisitEnter(AllocExpressionNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(AllocExpressionNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(ArrayTypeNode &node) override { DefaultVisit(node); }
  virtual void VisitLeave(ArrayTypeNode &node) override { DefaultVisit(node); }

  virtual void VisitEnter(CallStatementNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(CallStatementNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(CodeBlockNode &node) override { DefaultVisit(node); }
  virtual void VisitLeave(CodeBlockNode &node) override { DefaultVisit(node); }

  virtual void VisitEnter(DeallocStatementNode &node) { DefaultVisit(node); }
  virtual void VisitLeave(DeallocStatementNode &node) { DefaultVisit(node); }

  virtual void VisitEnter(DeclarationStatementNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(DeclarationStatementNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(ExpressionStatementNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(ExpressionStatementNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(ForStatementNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(ForStatementNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(FunctionDefinitionNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(FunctionDefinitionNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(IfStatementNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(IfStatementNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(LambdaInvocationNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(LambdaInvocationNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(LambdaNode &node) override { DefaultVisit(node); }
  virtual void VisitLeave(LambdaNode &node) override { DefaultVisit(node); }

  virtual void VisitEnter(LambdaParameterNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(LambdaParameterNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(LambdaTypeNode &node) override { DefaultVisit(node); }
  virtual void VisitLeave(LambdaTypeNode &node) override { DefaultVisit(node); }

  virtual void VisitEnter(MainNode &node) override { DefaultVisit(node); }
  virtual void VisitLeave(MainNode &node) override { DefaultVisit(node); }

  virtual void VisitEnter(OperatorNode &node) override { DefaultVisit(node); }
  virtual void VisitLeave(OperatorNode &node) override { DefaultVisit(node); }

  virtual void VisitEnter(PopulatedArrayExpressionNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(PopulatedArrayExpressionNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(PrintStatementNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(PrintStatementNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(ProgramNode &node) override { DefaultVisit(node); }
  virtual void VisitLeave(ProgramNode &node) override { DefaultVisit(node); }

  virtual void VisitEnter(ReturnStatementNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(ReturnStatementNode &node) override {
    DefaultVisit(node);
  }

  virtual void VisitEnter(WhileStatementNode &node) override {
    DefaultVisit(node);
  }
  virtual void VisitLeave(WhileStatementNode &node) override {
    DefaultVisit(node);
  }

  // ---- Leaf nodes
  // -----------------------------------------------------------------------
  virtual void Visit(ErrorNode &node) override { DefaultVisit(node); }
  virtual void Visit(NopNode &node) override { DefaultVisit(node); }
  virtual void Visit(IdentifierNode &node) override { DefaultVisit(node); }
  virtual void Visit(BooleanLiteralNode &node) override { DefaultVisit(node); }
  virtual void Visit(CharacterLiteralNode &node) override {
    DefaultVisit(node);
  }
  virtual void Visit(IntegerLiteralNode &node) override { DefaultVisit(node); }
  virtual void Visit(StringLiteralNode &node) override { DefaultVisit(node); }
  virtual void Visit(BaseTypeNode &node) override { DefaultVisit(node); }

 private:
  void DefaultVisit(ParseNode &node) {}
};

#endif  // PARSE_NODE_VISITOR_H_
