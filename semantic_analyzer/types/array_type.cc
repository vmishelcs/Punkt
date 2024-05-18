#include "array_type.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

#include <memory>
#include <string>

#include "arbitrary_type.h"

static const int kArrayTypeSizeInBytes = 8;

std::unique_ptr<ArrayType> ArrayType::CreateArrayType(Type *subtype) {
  return std::make_unique<ArrayType>(subtype);
}

std::unique_ptr<Type> ArrayType::CreateEquivalentType() const {
  return CreateArrayType(this->subtype.get());
}

bool ArrayType::IsEquivalentTo(Type *other_type) {
  ArrayType *other_arr_type = dynamic_cast<ArrayType *>(other_type);
  if (!other_arr_type) {
    return false;
  }

  return this->subtype->IsEquivalentTo(other_arr_type->subtype.get());
}

std::string ArrayType::ToString() const {
  return "[" + subtype->ToString() + "]";
}

void ArrayType::ResetArbitraryTypes() { subtype->ResetArbitraryTypes(); }

unsigned ArrayType::GetSizeInBytes() const { return kArrayTypeSizeInBytes; }

llvm::Type *ArrayType::GetLLVMType(llvm::LLVMContext &llvm_context) const {
  return llvm::PointerType::getUnqual(llvm_context);
}

ArrayType::ArrayType(Type *subtype) : Type(TypeEnum::ARRAY) {
  this->subtype = subtype->CreateEquivalentType();
}
