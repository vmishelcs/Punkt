#ifndef DECLARATION_NODE_H_
#define DECLARATION_NODE_H_

#include <parse_node/parse_node.h>
#include <token/token.h>

#include "identifier_node.h"

class DeclarationStatementNode : public ParseNode {
 public:
  DeclarationStatementNode(std::unique_ptr<Token> token);

  IdentifierNode *GetIdentifierNode() const {
    return dynamic_cast<IdentifierNode *>(GetChild(0));
  }
  ParseNode *GetInitializer() const { return GetChild(1); }

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // DECLARATION_NODE_H_
