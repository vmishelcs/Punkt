#ifndef SIGNATURES_H_
#define SIGNATURES_H_

#include <scanner/punctuator.h>

#include <vector>

#include "signature.h"

namespace signatures {
/// @brief Checks if the specified punctuator has a signature that matches
/// the provided input types.
/// @param punctuator `Punctuator` describing a punctuator (i.e.
/// operator).
/// @param types Operand types that the punctuator (or operator) should
/// accept.
/// @return Pointer to a `Signature` object if there exists a punctuator that
/// accepts the types provided in the `types` vector. Otherwise, returns
/// `nullptr`.
Signature *AcceptingSignature(Punctuator punctuator,
                              const std::vector<Type *> &types);
}  // namespace signatures

#endif  // SIGNATURES_H_
