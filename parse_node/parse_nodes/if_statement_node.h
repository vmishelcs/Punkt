#ifndef IF_STATEMENT_NODE_H_
#define IF_STATEMENT_NODE_H_

#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <token/token.h>

#include <memory>
#include <string>
#include <vector>

class IfStatementNode : public ParseNode {
 public:
  IfStatementNode(std::unique_ptr<Token> token)
      : ParseNode(ParseNodeType::IF_STATEMENT_NODE, std::move(token)) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  void AppendIf(std::unique_ptr<ParseNode> if_condition,
                std::unique_ptr<ParseNode> if_block);
  void AppendElse(std::unique_ptr<ParseNode> else_block);

  ParseNode *GetIfConditionNode() const { return GetChild(0); }
  ParseNode *GetIfBlockNode() const { return GetChild(1); }
  ParseNode *GetElseBlockNode() const { return GetChild(2); }

  bool HasElseBlock() const { return NumChildren() == 3; }

  virtual std::string ToString() const override { return "IF STATEMENT NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;
};

#endif  // IF_STATEMENT_NODE_H_
