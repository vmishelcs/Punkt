#ifndef SIGNATURES_H_
#define SIGNATURES_H_

#include <unordered_map>
#include <optional>
#include <functional>

#include <scanner/punctuator.h>

#include "signature.h"

class Signatures {
public:
    /// @brief Checks if the specified punctuator has a signature that matches the provided input
    /// types.
    /// @param punctuator `PunctuatorEnum` describing a punctuator (i.e. operator).
    /// @param types Operand types that the punctuator (or operator) should accept.
    /// @return Pointer to a `Signature` object if there exists a punctuator that accepts the types
    /// provided in the `types` vector. Otherwise, returns `nullptr`.
    static Signature const *AcceptingSignature(PunctuatorEnum punctuator,
            std::vector<Type *>& types);

private:
    static std::unordered_map<PunctuatorEnum, std::vector<Signature>> signature_map;
};

#endif // SIGNATURES_H_
