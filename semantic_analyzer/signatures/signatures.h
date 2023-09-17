#ifndef SIGNATURES_H_
#define SIGNATURES_H_

#include <unordered_map>
#include <optional>
#include <functional>

#include <scanner/punctuator.h>

#include "signature.h"

class Signatures {
public:
    static std::optional<std::reference_wrapper<const Signature>> AcceptingSignature(PunctuatorEnum punctuator, std::vector<std::reference_wrapper<const Type>>& types);

private:
    static std::unordered_map<PunctuatorEnum, std::vector<Signature>> signature_map;
};

#endif // SIGNATURES_H_
