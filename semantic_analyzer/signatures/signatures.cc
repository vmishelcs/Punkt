#include "signatures.h"

#include <code_generator/operator_codegen.h>
#include <semantic_analyzer/types/arbitrary_type.h>
#include <semantic_analyzer/types/base_type.h>

#include <memory>

#include "signature.h"

// Types used by signatures.
static const auto kArbitraryTypeT = std::make_unique<ArbitraryType>();
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

std::unordered_map<Punctuator, std::vector<Signature> >
    Signatures::signature_map{
        // =
        {Punctuator::EQUAL,
         {Signature({kArbitraryTypeT.get(), kArbitraryTypeT.get()},
                    kArbitraryTypeT.get(),
                    operator_codegen::AssignmentCodegen)}},
        // +
        {Punctuator::PLUS,
         {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                    operator_codegen::UnaryNop),
          Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    operator_codegen::IntegerAddCodegen)}},
        // -
        {Punctuator::MINUS,
         {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                    operator_codegen::IntegerNegationCodegen),
          Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    operator_codegen::IntegerSubtractCodegen)}},
        // *
        {Punctuator::MULTIPLY,
         {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    operator_codegen::IntegerMultiplyCodegen)}},
        // /
        {Punctuator::DIVIDE,
         {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                    kBaseTypeInteger.get(),
                    operator_codegen::IntegerDivideCodegen)}},
        // ==
        {Punctuator::CMP_EQ,
         {
             Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::BooleanCmpEQCodegen),
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpEQCodegen),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpEQCodegen),
         }},
        // !=
        {Punctuator::CMP_NEQ,
         {
             Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::BooleanCmpNEQCodegen),
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpNEQCodegen),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpNEQCodegen),
         }},
        // >
        {Punctuator::CMP_GT,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpGTCodegen),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpGTCodegen),
         }},
        // <
        {Punctuator::CMP_LT,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpLTCodegen),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpLTCodegen),
         }},
        // >=
        {Punctuator::CMP_GEQ,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpGEQCodegen),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpGEQCodegen),
         }},
        // <=
        {Punctuator::CMP_LEQ,
         {
             Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::CharacterCmpLEQCodegen),
             Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                       kBaseTypeBoolean.get(),
                       operator_codegen::IntegerCmpLEQCodegen),
         }}};

Signature *Signatures::AcceptingSignature(Punctuator punctuator,
                                          std::vector<Type *> &types) {
  std::vector<Signature> &signatures = signature_map.at(punctuator);
  for (Signature &signature : signatures) {
    if (signature.Accepts(types)) {
      return &signature;
    }
  }
  return nullptr;
}
