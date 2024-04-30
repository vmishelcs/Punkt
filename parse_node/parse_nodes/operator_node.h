#ifndef OPERATOR_NODE_H_
#define OPERATOR_NODE_H_

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <parse_node/parse_node.h>
#include <scanner/punctuator.h>
#include <token/punctuator_token.h>

#include <functional>
#include <variant>

using code_gen_function_variant =
    std::variant<llvm::Value *(*)(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *, llvm::Value *),
                 llvm::Value *(*)(llvm::LLVMContext *context,
                                  llvm::IRBuilder<> *, llvm::Value *,
                                  llvm::Value *)>;

class OperatorNode : public ParseNode {
 public:
  OperatorNode(std::unique_ptr<Token> token);

  inline Punctuator GetPunctuatorEnum() { return punctuator_enum; }

  inline void SetCodeGenFunc(code_gen_function_variant f) { this->f = f; }
  inline code_gen_function_variant GetCodeGenFunc() { return f; }

  virtual std::string ToString() const override;

  virtual void Accept(ParseNodeVisitor &visitor) override;

  virtual llvm::Value *GenerateCode(ParseNodeIRVisitor &visitor) override;

 private:
  Punctuator punctuator_enum;
  code_gen_function_variant f;
};

#endif  // OPERATOR_NODE_H_
