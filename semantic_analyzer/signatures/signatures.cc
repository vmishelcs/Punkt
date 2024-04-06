#include <code_generator/operator_code_generator.h>

#include "signatures.h"

std::unordered_map<PunctuatorEnum, std::vector<Signature>> Signatures::signature_map{
    {
        PunctuatorEnum::PLUS,
        {
            Signature({Type(TypeEnum::INTEGER)}, Type(TypeEnum::INTEGER), OperatorCodeGenerator::UnaryNoOp),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::INTEGER, OperatorCodeGenerator::IntegerAddCodeGenerator)
        }
    },
    {
        PunctuatorEnum::MINUS,
        {
            Signature({Type(TypeEnum::INTEGER)}, Type(TypeEnum::INTEGER), OperatorCodeGenerator::IntegerNegationCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::INTEGER, OperatorCodeGenerator::IntegerSubtractCodeGenerator)
        }
    },
    {
        PunctuatorEnum::MULTIPLY,
        {
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::INTEGER, OperatorCodeGenerator::IntegerMultiplyCodeGenerator)
        }
    },
    {
        PunctuatorEnum::DIVIDE,
        {
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::INTEGER, OperatorCodeGenerator::IntegerDivideCodeGenerator)
        }
    },
    {
        PunctuatorEnum::CMP_EQ,
        {
            Signature({Type(TypeEnum::BOOLEAN), Type(TypeEnum::BOOLEAN)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::BooleanCmpEqCodeGenerator),
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::CharacterCmpEQCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::IntegerCmpEQCodeGenerator),
        }
    },
    {
        PunctuatorEnum::CMP_NEQ,
        {
            Signature({Type(TypeEnum::BOOLEAN), Type(TypeEnum::BOOLEAN)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::BooleanCmpNeqCodeGenerator),
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::CharacterCmpNEQCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::IntegerCmpNEQCodeGenerator),
        }
    },
    {
        PunctuatorEnum::CMP_G,
        {
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::CharacterCmpGTCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::IntegerCmpGTCodeGenerator),
        }
    },
    {
        PunctuatorEnum::CMP_L,
        {
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::CharacterCmpLTCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::IntegerCmpLTCodeGenerator),
        }
    },
    {
        PunctuatorEnum::CMP_GEQ,
        {
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::CharacterCmpGEQCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::IntegerCmpGEQCodeGenerator),
        }
    },
    {
        PunctuatorEnum::CMP_LEQ,
        {
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::CharacterCmpLEQCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::IntegerCmpLEQCodeGenerator),
        }
    }
};

std::optional<std::reference_wrapper<const Signature>> Signatures::AcceptingSignature(PunctuatorEnum punctuator, std::vector<std::reference_wrapper<const Type>>& types) {
    auto& signatures = signature_map.at(punctuator);
    for (const Signature& signature : signatures) {
        if (signature.Accepts(types)) {
            return signature;
        }
    }
    return std::nullopt;
}
