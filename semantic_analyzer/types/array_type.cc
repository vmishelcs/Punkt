#include "array_type.h"

#include <memory>
#include <string>

#include "arbitrary_type.h"

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

ArrayType::ArrayType(Type *subtype) : Type(TypeEnum::ARRAY) {
  this->subtype = subtype->CreateEquivalentType();
}
