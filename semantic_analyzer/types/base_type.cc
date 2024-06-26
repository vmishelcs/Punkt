#include "base_type.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <logging/punkt_logger.h>

static const unsigned kVoidTypeSizeInBytes = 0;
static const unsigned kBooleanTypeSizeInBytes = 1;
static const unsigned kCharacterTypeSizeInBytes = 1;
static const unsigned kIntegerTypeSizeInBytes = 8;
static const unsigned kFloatTypeSizeInBytes = 8;
static const unsigned kRationalTypeSizeInBytes = 16;
static const unsigned kStringTypeSizeInBytes = 8;
static const unsigned kErrorTypeSizeInBytes = 0;

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

std::unique_ptr<BaseType> BaseType::CreateFloatType() {
  return Create(BaseTypeEnum::FLOAT);
}

std::unique_ptr<BaseType> BaseType::CreateRationalType() {
  return Create(BaseTypeEnum::RATIONAL);
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

unsigned BaseType::GetSizeInBytes() const {
  switch (base_type_enum) {
    case BaseTypeEnum::VOID:
      return kVoidTypeSizeInBytes;
    case BaseTypeEnum::BOOLEAN:
      return kBooleanTypeSizeInBytes;
    case BaseTypeEnum::CHARACTER:
      return kCharacterTypeSizeInBytes;
    case BaseTypeEnum::INTEGER:
      return kIntegerTypeSizeInBytes;
    case BaseTypeEnum::FLOAT:
      return kFloatTypeSizeInBytes;
    case BaseTypeEnum::RATIONAL:
      return kRationalTypeSizeInBytes;
    case BaseTypeEnum::STRING:
      return kStringTypeSizeInBytes;
    case BaseTypeEnum::ERROR:
      return kErrorTypeSizeInBytes;
    default:
      PunktLogger::LogFatalInternalError(
          "unimplemented BaseType in BaseType::GetSizeInBytes");
  }
}

llvm::Type *BaseType::GetLLVMType(llvm::LLVMContext &llvm_context) const {
  switch (base_type_enum) {
    case BaseTypeEnum::VOID:
      return llvm::Type::getVoidTy(llvm_context);
    case BaseTypeEnum::BOOLEAN:
      return llvm::Type::getInt8Ty(llvm_context);
    case BaseTypeEnum::CHARACTER:
      return llvm::Type::getInt8Ty(llvm_context);
    case BaseTypeEnum::INTEGER:
      return llvm::Type::getInt64Ty(llvm_context);
    case BaseTypeEnum::FLOAT:
      return llvm::Type::getDoubleTy(llvm_context);
    case BaseTypeEnum::RATIONAL:
      return llvm::Type::getInt128Ty(llvm_context);
    case BaseTypeEnum::STRING:
      return llvm::PointerType::getUnqual(llvm_context);
    case BaseTypeEnum::ERROR:
      PunktLogger::LogFatalInternalError(
          "error type has no corresponding LLVM type");
    default:
      PunktLogger::LogFatalInternalError(
          "unimplemented BaseType in BaseType::GetLLVMType");
  }
}

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
    case BaseTypeEnum::FLOAT:
      return "float";
    case BaseTypeEnum::RATIONAL:
      return "rational";
    case BaseTypeEnum::STRING:
      return "string";
    case BaseTypeEnum::ERROR:
      return "error";
    default:
      PunktLogger::LogFatalInternalError(
          "unimplemented BaseType in BaseType::GetEnumString");
  }
}
