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
