#ifndef CUSTOM_TYPE_H_
#define CUSTOM_TYPE_H_

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

#include <memory>
#include <string>
#include <vector>

#include "type.h"

/// @brief This class represents user-defined types within the Punkt program and
/// implements utility methods for representing said user-defined types within
/// the Punkt compiler.
class CustomType : public Type {
 public:
  CustomType(const std::vector<Type *> &subtypes);

  static std::unique_ptr<CustomType> CreateCustomType(
      const std::string &type_name, const std::vector<Type *> &subtypes);

  virtual std::unique_ptr<Type> CreateEquivalentType() const override;

  virtual bool IsEquivalentTo(Type *other_type) override;

  virtual std::string ToString() const override;

  virtual bool IsErrorType() const override { return false; }

  virtual void ResetArbitraryTypes() override {}

  virtual unsigned GetSizeInBytes() const override;
  virtual llvm::Type *GetLLVMType(
      llvm::LLVMContext &llvm_context) const override;

 private:
  std::string type_name;
  std::vector<Type *> subtypes;
};

#endif  // CUSTOM_TYPE_H_
