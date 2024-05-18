#ifndef PARSE_NODE_H_
#define PARSE_NODE_H_

#include <llvm/IR/Value.h>
#include <semantic_analyzer/types/type.h>
#include <symbol_table/scope.h>
#include <token/token.h>

#include <memory>
#include <vector>

// Forward-declare ParseNodeVisitor to avoid circular dependencies
class ParseNodeVisitor;

// Forward-declare ParseNodeIRVisitor to avoid circular dependencies
class ParseNodeIRVisitor;

enum class ParseNodeType {
  ALLOC_STATEMENT_NODE,
  ARRAY_TYPE_NODE,
  BASE_TYPE_NODE,
  BOOLEAN_LITERAL_NODE,
  CALL_STATEMENT_NODE,
  CHARACTER_LITERAL_NODE,
  CODE_BLOCK_NODE,
  DEALLOC_STATEMENT_NODE,
  DECLARATION_STATEMENT_NODE,
  ERROR_NODE,
  EXPRESSION_STATEMENT_NODE,
  FOR_STATEMENT_NODE,
  FUNCTION_DEFINITION_NODE,
  IDENTIFIER_NODE,
  IF_STATEMENT_NODE,
  INTEGER_LITERAL_NODE,
  LAMBDA_INVOCATION_NODE,
  LAMBDA_NODE,
  LAMBDA_PARAMETER_NODE,
  LAMBDA_TYPE_NODE,
  MAIN_NODE,
  OPERATOR_NODE,
  POPULATED_ARRAY_EXPRESSION_NODE,
  PRINT_STATEMENT_NODE,
  PROGRAM_NODE,
  RETURN_STATEMENT_NODE,
  STRING_LITERAL_NODE,
  WHILE_STATEMENT_NODE,
};

/// @brief Class to represent a node of the abstract syntax tree.
class ParseNode {
 public:
  ParseNode(ParseNodeType node_type, std::unique_ptr<Token> token);

  /// @brief Creates a copy of the subtree defined by this node.
  /// @return A `unique_ptr` to a `ParseNode` object that is an independent copy
  /// of this node, along with any descendants.
  /// @warning This method does not initialize any semantic information (e.g.
  /// type, scope, etc) of the returned copy. Hence, this method should only be
  /// used by the parser.
  virtual std::unique_ptr<ParseNode> CreateCopy() const = 0;

  ParseNodeType GetParseNodeType() const { return node_type; }

  Token *GetToken() const { return token.get(); }
  const TextLocation &GetTextLocation() const { return text_location; }

  ParseNode *GetParent() const { return parent; }
  unsigned NumChildren() const;
  ParseNode *GetChild(unsigned i) const;
  std::vector<ParseNode *> GetChildren() const;
  void AppendChild(std::unique_ptr<ParseNode> node);

  std::vector<ParseNode *> GetPathToRoot();

  virtual std::string ToString() const = 0;

  virtual void Accept(ParseNodeVisitor &visitor) = 0;

  bool HasType() const;
  Type *GetType() const;
  void SetType(std::unique_ptr<Type> type);

  bool HasScope() const;
  Scope *GetScope() const;
  void SetScope(std::unique_ptr<Scope> scope);

  Scope *GetLocalScope();

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) = 0;

 protected:
  void VisitChildren(ParseNodeVisitor &visitor);

  std::unique_ptr<Token> token;
  TextLocation text_location;

 private:
  ParseNodeType node_type;

  ParseNode *parent;
  std::vector<std::unique_ptr<ParseNode> > children;

  // Type that describes this node, if any
  std::unique_ptr<Type> type;

  // Scope created by this node; not every node creates a scope
  std::unique_ptr<Scope> scope;
};

#endif  // PARSE_NODE_H_
