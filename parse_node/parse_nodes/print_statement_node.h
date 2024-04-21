#ifndef PRINT_STATEMENT_NODE_H_
#define PRINT_STATEMENT_NODE_H_

#include <parse_node/parse_node.h>

class PrintStatementNode : public ParseNode {
 public:
  PrintStatementNode(std::unique_ptr<Token> token);

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // PRINT_STATEMENT_NODE_H_
