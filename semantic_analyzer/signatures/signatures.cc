#include "signatures.h"

#include <code_generator/operator_codegen.h>

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

std::unordered_map<PunctuatorEnum, std::vector<Signature> >
    Signatures::signature_map{
        {PunctuatorEnum::PLUS,
         {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                    operator_codegen::UnaryNop),
          Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    operator_codegen::IntegerAddCodeGenerator)}},
        {PunctuatorEnum::MINUS,
         {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                    operator_codegen::IntegerNegationCodeGenerator),
          Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    operator_codegen::IntegerSubtractCodeGenerator)}},
        {PunctuatorEnum::MULTIPLY,
         {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    operator_codegen::IntegerMultiplyCodeGenerator)}},
        {PunctuatorEnum::DIVIDE,
         {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    operator_codegen::IntegerDivideCodeGenerator)}},
        {PunctuatorEnum::CMP_EQ,
         {
             Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::BooleanCmpEQCodeGenerator),
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpEQCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpEQCodeGenerator),
         }},
        {PunctuatorEnum::CMP_NEQ,
         {
             Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::BooleanCmpNEQCodeGenerator),
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpNEQCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpNEQCodeGenerator),
         }},
        {PunctuatorEnum::CMP_GT,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpGTCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpGTCodeGenerator),
         }},
        {PunctuatorEnum::CMP_LT,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpLTCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpLTCodeGenerator),
         }},
        {PunctuatorEnum::CMP_GEQ,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpGEQCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpGEQCodeGenerator),
         }},
        {PunctuatorEnum::CMP_LEQ,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpLEQCodeGenerator),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpLEQCodeGenerator),
         }}};

Signature const *Signatures::AcceptingSignature(PunctuatorEnum punctuator,
                                                std::vector<Type *> &types) {
  const auto &signatures = signature_map.at(punctuator);
  for (const Signature &signature : signatures) {
    if (signature.Accepts(types)) {
      return &signature;
    }
  }
  return nullptr;
}
