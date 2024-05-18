#ifndef SIGNATURES_H_
#define SIGNATURES_H_

#include <scanner/punctuator.h>

#include <vector>

#include "signature.h"

namespace signatures {
/// @brief Checks if the specified operator has a signature that matches
/// the provided input types.
/// @param op `Operator` enum describing an operator.
/// @param types Operand types that the operator should
/// accept.
/// @return Pointer to a `Signature` object if there exists an operator that
/// accepts the types provided in the `types` vector. Otherwise, returns
/// `nullptr`.
Signature *AcceptingSignature(Operator op, const std::vector<Type *> &types);
}  // namespace signatures

#endif  // SIGNATURES_H_
