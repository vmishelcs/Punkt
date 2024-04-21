#include "signatures.h"

#include <code_generator/operator_code_generator.h>

#include <memory>

#include "signature.h"

// Types used by signatures.
static const std::unique_ptr<BaseType> kBaseTypeNull =
    BaseType::CreateVoidType();
static const std::unique_ptr<BaseType> kBaseTypeBoolean =
    BaseType::CreateBooleanType();
static const std::unique_ptr<BaseType> kBaseTypeCharacter =
    BaseType::CreateCharacterType();
static const std::unique_ptr<BaseType> kBaseTypeInteger =
    BaseType::CreateIntegerType();
static const std::unique_ptr<BaseType> kBaseTypeString =
    BaseType::CreateStringType();

std::unordered_map<PunctuatorEnum, std::vector<Signature>>
    Signatures::signature_map{
        {PunctuatorEnum::PLUS,
         {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                    OperatorCodeGenerator::UnaryNoOp),
          Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    OperatorCodeGenerator::IntegerAddCodeGenerator)}},
        {PunctuatorEnum::MINUS,
         {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                    OperatorCodeGenerator::IntegerNegationCodeGenerator),
          Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    OperatorCodeGenerator::IntegerSubtractCodeGenerator)}},
        {PunctuatorEnum::MULTIPLY,
         {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    OperatorCodeGenerator::IntegerMultiplyCodeGenerator)}},
        {PunctuatorEnum::DIVIDE,
         {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    OperatorCodeGenerator::IntegerDivideCodeGenerator)}},
        {PunctuatorEnum::CMP_EQ,
         {
             Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::BooleanCmpEqCodeGenerator),
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::CharacterCmpEQCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::IntegerCmpEQCodeGenerator),
         }},
        {PunctuatorEnum::CMP_NEQ,
         {
             Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::BooleanCmpNeqCodeGenerator),
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::CharacterCmpNEQCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::IntegerCmpNEQCodeGenerator),
         }},
        {PunctuatorEnum::CMP_G,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::CharacterCmpGTCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::IntegerCmpGTCodeGenerator),
         }},
        {PunctuatorEnum::CMP_L,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::CharacterCmpLTCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::IntegerCmpLTCodeGenerator),
         }},
        {PunctuatorEnum::CMP_GEQ,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::CharacterCmpGEQCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::IntegerCmpGEQCodeGenerator),
         }},
        {PunctuatorEnum::CMP_LEQ,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::CharacterCmpLEQCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       OperatorCodeGenerator::IntegerCmpLEQCodeGenerator),
         }}};

Signature const* Signatures::AcceptingSignature(PunctuatorEnum punctuator,
                                                std::vector<Type*>& types) {
  const auto& signatures = signature_map.at(punctuator);
  for (const Signature& signature : signatures) {
    if (signature.Accepts(types)) {
      return &signature;
    }
  }
  return nullptr;
}
