#include "signatures.h"

#include <code_generator/operator_codegen.h>
#include <scanner/operator.h>
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
// Rational type.
static const std::unique_ptr<BaseType> kBaseTypeRational =
    BaseType::CreateRationalType();
// String type.
static const std::unique_ptr<BaseType> kBaseTypeString =
    BaseType::CreateStringType();

static std::map<Operator, std::vector<Signature> > signature_map = {
    // =
    {Operator::ASSIGN,
     {Signature({kArbitraryTypeT.get(), kArbitraryTypeT.get()},
                kBaseTypeVoid.get(), operator_codegen::AssignmentCodegen)}},
    // +
    {Operator::PLUS,
     {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                operator_codegen::UnaryNop),
      Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(), operator_codegen::IntegerAddCodegen),
      Signature({kBaseTypeRational.get(), kBaseTypeRational.get()},
                kBaseTypeRational.get(),
                operator_codegen::RationalAddCodegen)}},
    // -
    {Operator::MINUS,
     {Signature({kBaseTypeInteger.get()}, kBaseTypeInteger.get(),
                operator_codegen::IntegerNegationCodegen),
      Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(),
                operator_codegen::IntegerSubtractCodegen),
      Signature({kBaseTypeRational.get(), kBaseTypeRational.get()},
                kBaseTypeRational.get(),
                operator_codegen::RationalSubtractCodegen)}},
    // *
    {Operator::MUL,
     {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(),
                operator_codegen::IntegerMultiplyCodegen),
      Signature({kBaseTypeRational.get(), kBaseTypeRational.get()},
                kBaseTypeRational.get(),
                operator_codegen::RationalMultiplyCodegen)}},
    // /
    {Operator::DIV,
     {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(), operator_codegen::IntegerDivideCodegen),
      Signature({kBaseTypeRational.get(), kBaseTypeRational.get()},
                kBaseTypeRational.get(),
                operator_codegen::RationalDivideCodegen)}},
    // %
    {Operator::MOD,
     {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeInteger.get(),
                operator_codegen::IntegerModuloCodegen)}},
    // //
    {Operator::OVER,
     {Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeRational.get(),
                operator_codegen::OverOperatorCodegen)}},
    // ==
    {Operator::CMP_EQ,
     {Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                kBaseTypeBoolean.get(), operator_codegen::BooleanCmpEQCodegen),
      Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                kBaseTypeBoolean.get(),
                operator_codegen::CharacterCmpEQCodegen),
      Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                kBaseTypeBoolean.get(), operator_codegen::IntegerCmpEQCodegen),
      Signature({kBaseTypeRational.get(), kBaseTypeRational.get()},
                kBaseTypeBoolean.get(),
                operator_codegen::RationalCmpEQCodegen)}},
    // !=
    {Operator::CMP_NEQ,
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
    {Operator::CMP_GT,
     {
         Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                   kBaseTypeBoolean.get(),
                   operator_codegen::CharacterCmpGTCodegen),
         Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                   kBaseTypeBoolean.get(),
                   operator_codegen::IntegerCmpGTCodegen),
     }},
    // <
    {Operator::CMP_LT,
     {
         Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                   kBaseTypeBoolean.get(),
                   operator_codegen::CharacterCmpLTCodegen),
         Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                   kBaseTypeBoolean.get(),
                   operator_codegen::IntegerCmpLTCodegen),
     }},
    // >=
    {Operator::CMP_GEQ,
     {
         Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                   kBaseTypeBoolean.get(),
                   operator_codegen::CharacterCmpGEQCodegen),
         Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                   kBaseTypeBoolean.get(),
                   operator_codegen::IntegerCmpGEQCodegen),
     }},
    // <=
    {Operator::CMP_LEQ,
     {
         Signature({kBaseTypeCharacter.get(), kBaseTypeCharacter.get()},
                   kBaseTypeBoolean.get(),
                   operator_codegen::CharacterCmpLEQCodegen),
         Signature({kBaseTypeInteger.get(), kBaseTypeInteger.get()},
                   kBaseTypeBoolean.get(),
                   operator_codegen::IntegerCmpLEQCodegen),
     }},
    // !
    {Operator::BOOL_NOT,
     {Signature({kBaseTypeBoolean.get()}, kBaseTypeBoolean.get(),
                operator_codegen::BooleanNegationCodegen)}},
    // &&
    {Operator::BOOL_AND,
     {Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                kBaseTypeBoolean.get(), operator_codegen::BooleanAndCodegen)}},
    // ||
    {Operator::BOOL_OR,
     {Signature({kBaseTypeBoolean.get(), kBaseTypeBoolean.get()},
                kBaseTypeBoolean.get(), operator_codegen::BooleanOrCodegen)}},
    // []
    {Operator::ARRAY_IDX,
     {Signature({kArbitraryArrayTypeT.get(), kBaseTypeInteger.get()},
                kArbitraryArrayTypeT->GetSubtype(),
                operator_codegen::ArrayIndexingCodegen)}},
    // sizeof
    {Operator::SIZEOF,
     {Signature({kArbitraryArrayTypeT.get()}, kBaseTypeInteger.get(),
                operator_codegen::ArraySizeofCodegen)}}};

Signature *signatures::AcceptingSignature(Operator op,
                                          const std::vector<Type *> &types) {
  std::vector<Signature> &signatures = signature_map.at(op);
  for (Signature &signature : signatures) {
    if (signature.Accepts(types)) {
      return &signature;
    }
  }
  return nullptr;
}
