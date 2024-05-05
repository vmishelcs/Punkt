#include "base_type.h"

#include <logging/punkt_logger.h>

std::unique_ptr<BaseType> BaseType::Create(BaseTypeEnum base_type_enum) {
  return std::make_unique<BaseType>(base_type_enum);
}

std::unique_ptr<BaseType> BaseType::CreateVoidType() {
  return Create(BaseTypeEnum::VOID);
}

std::unique_ptr<BaseType> BaseType::CreateBooleanType() {
  return Create(BaseTypeEnum::BOOLEAN);
}

std::unique_ptr<BaseType> BaseType::CreateCharacterType() {
  return Create(BaseTypeEnum::CHARACTER);
}

std::unique_ptr<BaseType> BaseType::CreateIntegerType() {
  return Create(BaseTypeEnum::INTEGER);
}

std::unique_ptr<BaseType> BaseType::CreateStringType() {
  return Create(BaseTypeEnum::STRING);
}

std::unique_ptr<BaseType> BaseType::CreateErrorType() {
  return Create(BaseTypeEnum::ERROR);
}

bool BaseType::IsEquivalentTo(BaseTypeEnum base_type_enum) const {
  return this->base_type_enum == base_type_enum;
}

bool BaseType::IsEquivalentTo(Type *other_type) {
  BaseType *other_base_type = dynamic_cast<BaseType *>(other_type);
  if (!other_base_type) {
    return false;
  }

  return other_base_type->base_type_enum == this->base_type_enum;
}

std::unique_ptr<Type> BaseType::CreateEquivalentType() const {
  return std::make_unique<BaseType>(this->base_type_enum);
}

BaseType::BaseType(BaseTypeEnum base_type_enum)
    : Type(TypeEnum::BASE_TYPE), base_type_enum(base_type_enum) {}

std::string BaseType::GetEnumString(BaseTypeEnum base_type_enum) {
  switch (base_type_enum) {
    case BaseTypeEnum::VOID:
      return "void";
    case BaseTypeEnum::BOOLEAN:
      return "boolean";
    case BaseTypeEnum::CHARACTER:
      return "character";
    case BaseTypeEnum::INTEGER:
      return "integer";
    case BaseTypeEnum::STRING:
      return "string";
    case BaseTypeEnum::ERROR:
      return "error";
    default:
      return (const char *)PunktLogger::LogFatalInternalError(
          "unimplemented BaseType in BaseType::GetEnumString");
  }
}
