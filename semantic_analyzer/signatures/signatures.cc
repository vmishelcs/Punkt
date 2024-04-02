#include "signatures.h"

std::unordered_map<PunctuatorEnum, std::vector<Signature>> Signatures::signature_map{
    {
        PunctuatorEnum::PLUS,
        {
            Signature({Type(TypeEnum::INTEGER)}, Type(TypeEnum::INTEGER)),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::INTEGER)
        }
    },
    {
        PunctuatorEnum::MINUS,
        {
            Signature({Type(TypeEnum::INTEGER)}, Type(TypeEnum::INTEGER)),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::INTEGER)
        }
    },
    {
        PunctuatorEnum::MULTIPLY,
        {
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::INTEGER)
        }
    },
    {
        PunctuatorEnum::DIVIDE,
        {
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::INTEGER)
        }
    },
    {
        PunctuatorEnum::CMP_EQ,
        {
            Signature({Type(TypeEnum::BOOLEAN), Type(TypeEnum::BOOLEAN)}, TypeEnum::BOOLEAN),
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
        }
    },
    {
        PunctuatorEnum::CMP_NEQ,
        {
            Signature({Type(TypeEnum::BOOLEAN), Type(TypeEnum::BOOLEAN)}, TypeEnum::BOOLEAN),
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
        }
    },
    {
        PunctuatorEnum::CMP_G,
        {
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
        }
    },
    {
        PunctuatorEnum::CMP_L,
        {
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
        }
    },
    {
        PunctuatorEnum::CMP_GEQ,
        {
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
        }
    },
    {
        PunctuatorEnum::CMP_LEQ,
        {
            Signature({Type(TypeEnum::CHARACTER), Type(TypeEnum::CHARACTER)}, TypeEnum::BOOLEAN),
            Signature({Type(TypeEnum::INTEGER), Type(TypeEnum::INTEGER)}, TypeEnum::BOOLEAN),
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
