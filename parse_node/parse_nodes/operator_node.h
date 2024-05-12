#ifndef OPERATOR_NODE_H_
#define OPERATOR_NODE_H_

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <scanner/operator.h>

#include <memory>
#include <string>

// Forward-declare types.
class CodeGenerationVisitor;
class OperatorNode;

using codegen_function_type = llvm::Value *(*)(CodeGenerationVisitor &,
                                               OperatorNode &);

class OperatorNode : public ParseNode {
 public:
  OperatorNode(std::unique_ptr<Token> token);

  virtual std::unique_ptr<ParseNode> CreateCopy() const override;

  Operator GetOperatorEnum() const { return op; }

  void SetCodegenFunction(codegen_function_type codegen_function) {
    this->codegen_function = codegen_function;
  }
  codegen_function_type GetCodegenFunction() const { return codegen_function; }

  virtual std::string ToString() const override { return "OPERATOR NODE"; }

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  Operator op;
  codegen_function_type codegen_function;
};

#endif  // OPERATOR_NODE_H_
