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
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::CharacterCmpEqCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::IntegerCmpEqCodeGenerator),
        }
    },
    {
        PunctuatorEnum::CMP_NEQ,
        {
            Signature({Type(TypeEnum::BOOLEAN), Type(TypeEnum::BOOLEAN)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::BooleanCmpNeqCodeGenerator),
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::CharacterCmpNeqCodeGenerator),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN, OperatorCodeGenerator::IntegerCmpNeqCodeGenerator),
        }
    },
    // {
    //     PunctuatorEnum::CMP_G,
    //     {
    //         Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
    //         Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
    //     }
    // },
    // {
    //     PunctuatorEnum::CMP_L,
    //     {
    //         Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
    //         Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
    //     }
    // },
    // {
    //     PunctuatorEnum::CMP_GEQ,
    //     {
    //         Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
    //         Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
    //     }
    // },
    // {
    //     PunctuatorEnum::CMP_LEQ,
    //     {
    //         Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
    //         Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
    //     }
    // }
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
