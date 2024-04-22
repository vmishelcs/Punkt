#include "arbitrary_type.h"

std::unique_ptr<Type> ArbitraryType::CreateEquivalentType() const {
  return set_type->CreateEquivalentType();
}

bool ArbitraryType::IsEquivalentTo(Type *other_type) {
  // If the `set_type` data member hasn't been set, set it and return true.
  if (!set_type) {
    set_type = other_type;
    return true;
  }

  // Otherwise make sure the set type is equivalent.
  return set_type->IsEquivalentTo(other_type);
}
