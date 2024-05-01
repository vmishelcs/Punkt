#ifndef PROGRAM_NODE_H_
#define PROGRAM_NODE_H_

#include <input_handler/text_location.h>
#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>

#include <memory>
#include <string>

class ProgramNode : public ParseNode {
 public:
  ProgramNode(TextLocation text_location)
      : ParseNode(ParseNodeType::PROGRAM_NODE, text_location) {}

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  std::string GetModuleID() const { return module_id; }
  void SetModuleID(const std::string module_id);

  virtual std::string ToString() const override { return "PROGRAM NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  std::string module_id;
};

#endif  // PROGRAM_NODE_H_
