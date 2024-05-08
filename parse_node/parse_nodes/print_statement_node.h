#ifndef PRINT_STATEMENT_NODE_H_
#define PRINT_STATEMENT_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>

class PrintStatementNode : public ParseNode {
 public:
  PrintStatementNode(std::unique_ptr<Token> token, bool is_println)
      : ParseNode(ParseNodeType::PRINT_STATEMENT_NODE, std::move(token)),
        is_println(is_println) {}

  bool IsPrintln() const { return is_println; }

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  virtual std::string ToString() const override {
    return "PRINT STATEMENT NODE";
  }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  bool is_println;
};

#endif  // PRINT_STATEMENT_NODE_H_
