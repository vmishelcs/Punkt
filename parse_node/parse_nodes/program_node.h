#ifndef PROGRAM_NODE_H_
#define PROGRAM_NODE_H_

#include <parse_node/parse_node.h>

class ProgramNode : public ParseNode {
 public:
  ProgramNode(TextLocation text_location)
      : ParseNode(ParseNodeType::PROGRAM_NODE, text_location) {}

  std::string GetModuleID() const;
  void SetModuleID(const std::string module_id);

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  std::string module_id;
};

#endif  // PROGRAM_NODE_H_
