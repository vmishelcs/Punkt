#include "signatures.h"

#include <code_generator/operator_codegen.h>
#include <semantic_analyzer/types/arbitrary_type.h>
#include <semantic_analyzer/types/array_type.h>
#include <semantic_analyzer/types/base_type.h>

#include <map>
#include <memory>

#include "signature.h"

// Types used by signatures:
// Arbitrary type T.
static const auto kArbitraryTypeT = std::make_unique<ArbitraryType>();
// Arbitrary type [T].
static const std::unique_ptr<ArrayType> kArbitraryArrayTypeT =
    ArrayType::CreateArrayType(kArbitraryTypeT.get());
// Void type.
static const std::unique_ptr<BaseType> kBaseTypeVoid =
    BaseType::CreateVoidType();
// Boolean type.
static const std::unique_ptr<BaseType> kBaseTypeBoolean =
    BaseType::CreateBooleanType();
// Character type.
static const std::unique_ptr<BaseType> kBaseTypeCharacter =
    BaseType::CreateCharacterType();
// Integer type.
static const std::unique_ptr<BaseType> kBaseTypeInteger =
    BaseType::CreateIntegerType();
// String type.
static const std::unique_ptr<BaseType> kBaseTypeString =
    BaseType::CreateStringType();

static std::map<Punctuator, std::vector<Signature> > signature_map = {
    // =
    {Punctuator::ASSIGN,
     {Signature({kArbitraryTypeT.get(), kArbitraryTypeT.get()},
                kArbitraryTypeT.get(), operator_codegen::AssignmentCodegen)}},
    // +
    {Punctuator::PLUS,
     {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                operator_codegen::UnaryNop),
      Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(), operator_codegen::IntegerAddCodegen)}},
    // -
    {Punctuator::MINUS,
     {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                operator_codegen::IntegerNegationCodegen),
      Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(),
                operator_codegen::IntegerSubtractCodegen)}},
    // *
    {Punctuator::MUL,
     {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(),
                operator_codegen::IntegerMultiplyCodegen)}},
    // /
    {Punctuator::DIV,
     {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(),
                operator_codegen::IntegerDivideCodegen)}},
    // %
    {Punctuator::MOD,
     {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(),
                operator_codegen::IntegerModuloCodegen)}},
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
     }},
    // !
    {Punctuator::BOOL_NOT,
     {Signature({kBaseTypeBoolean.get()}, kBaseTypeBoolean.get(),
                operator_codegen::BooleanNegationCodegen)}},
    // &&
    {Punctuator::BOOL_AND,
     {Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                kBaseTypeBoolean.get(), operator_codegen::BooleanAndCodegen)}},
    // ||
    {Punctuator::BOOL_OR,
     {Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                kBaseTypeBoolean.get(), operator_codegen::BooleanOrCodegen)}},
    // alloc
    {Punctuator::ALLOC,
     {Signature({kArbitraryArrayTypeT.get(), kBaseTypeInteger.get()},
                kArbitraryArrayTypeT.get(), operator_codegen::AllocCodegen)}}};

Signature *signatures::AcceptingSignature(Punctuator punctuator,
                                          const std::vector<Type *> &types) {
  std::vector<Signature> &signatures = signature_map.at(punctuator);
  for (Signature &signature : signatures) {
    if (signature.Accepts(types)) {
      return &signature;
    }
  }
  return nullptr;
}
