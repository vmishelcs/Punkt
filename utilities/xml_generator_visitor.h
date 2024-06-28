#ifndef XML_GENERATOR_VISITOR_H_
#define XML_GENERATOR_VISITOR_H_

#include <parse_node/parse_node_visitor.h>
#include <parse_node/parse_nodes/all_nodes.h>

#include <filesystem>
#include <iostream>
#include <map>

namespace fs = std::filesystem;

class XMLTag {
 private:
  enum class Type { START, END, EMPTY_ELEMENT };

 public:
  friend std::unique_ptr<XMLTag> std::make_unique<XMLTag>(std::string &,
                                                          Type &&);

  static std::unique_ptr<XMLTag> CreateStartTag(std::string name);
  static std::unique_ptr<XMLTag> CreateEndTag(std::string name);
  static std::unique_ptr<XMLTag> CreateSelfClosingTag(std::string name);

  void AddAttribute(std::string name, std::string value);

  std::string ToString() const;

 private:
  XMLTag(std::string name, Type tag_type);

  std::string name;
  Type tag_type;
  std::map<std::string, std::string> attributes;
};

class XMLGeneratorVisitor : public ParseNodeVisitor {
 public:
  XMLGeneratorVisitor(std::ostream &output_stream);

  // ---- Non-leaf nodes -------------------------------------------------------
  virtual void VisitEnter(AllocExpressionNode &node) override;
  virtual void VisitLeave(AllocExpressionNode &node) override;

  virtual void VisitEnter(ArrayTypeNode &node) override;
  virtual void VisitLeave(ArrayTypeNode &node) override;

  virtual void VisitEnter(CodeBlockNode &node) override;
  virtual void VisitLeave(CodeBlockNode &node) override;

  virtual void VisitEnter(DeallocStatementNode &node) override;
  virtual void VisitLeave(DeallocStatementNode &node) override;

  virtual void VisitEnter(DeclarationStatementNode &node) override;
  virtual void VisitLeave(DeclarationStatementNode &node) override;

  virtual void VisitEnter(ExpressionStatementNode &node) override;
  virtual void VisitLeave(ExpressionStatementNode &node) override;

  virtual void VisitEnter(ForStatementNode &node) override;
  virtual void VisitLeave(ForStatementNode &node) override;

  virtual void VisitEnter(FunctionDefinitionNode &node) override;
  virtual void VisitLeave(FunctionDefinitionNode &node) override;

  virtual void VisitEnter(IfStatementNode &node) override;
  virtual void VisitLeave(IfStatementNode &node) override;

  virtual void VisitEnter(LambdaInvocationNode &node) override;
  virtual void VisitLeave(LambdaInvocationNode &node) override;

  virtual void VisitEnter(LambdaNode &node) override;
  virtual void VisitLeave(LambdaNode &node) override;

  virtual void VisitEnter(LambdaParameterNode &node) override;
  virtual void VisitLeave(LambdaParameterNode &node) override;

  virtual void VisitEnter(LambdaTypeNode &node) override;
  virtual void VisitLeave(LambdaTypeNode &node) override;

  virtual void VisitEnter(MainNode &node) override;
  virtual void VisitLeave(MainNode &node) override;

  virtual void VisitEnter(OperatorNode &node) override;
  virtual void VisitLeave(OperatorNode &node) override;

  virtual void VisitEnter(PopulatedArrayExpressionNode &node) override;
  virtual void VisitLeave(PopulatedArrayExpressionNode &node) override;

  virtual void VisitEnter(PrintStatementNode &node) override;
  virtual void VisitLeave(PrintStatementNode &node) override;

  virtual void VisitEnter(ProgramNode &node) override;
  virtual void VisitLeave(ProgramNode &node) override;

  virtual void VisitEnter(ReturnStatementNode &node) override;
  virtual void VisitLeave(ReturnStatementNode &node) override;

  virtual void VisitEnter(WhileStatementNode &node) override;
  virtual void VisitLeave(WhileStatementNode &node) override;

  // ---- Leaf nodes -----------------------------------------------------------
  virtual void Visit(ErrorNode &node) override;
  virtual void Visit(NopNode &node) override;
  virtual void Visit(IdentifierNode &node) override;
  virtual void Visit(BooleanLiteralNode &node) override;
  virtual void Visit(CharacterLiteralNode &node) override;
  virtual void Visit(IntegerLiteralNode &node) override;
  virtual void Visit(FloatLiteralNode &node) override;
  virtual void Visit(StringLiteralNode &node) override;
  virtual void Visit(BaseTypeNode &node) override;

 private:
  int depth;
  void AddBasicParseNodeAttributes(XMLTag &tag, ParseNode &node);
  void OutputTag(XMLTag &tag);

  std::ostream &output_stream;
};

#endif  // XML_GENERATOR_VISITOR_H_
