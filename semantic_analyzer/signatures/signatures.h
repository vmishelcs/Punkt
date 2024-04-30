#ifndef SIGNATURES_H_
#define SIGNATURES_H_

#include <scanner/punctuator.h>

#include <unordered_map>

#include "signature.h"

// TODO: This doesn't need to be a class.
class Signatures {
 public:
  /// @brief Checks if the specified punctuator has a signature that matches
  /// the provided input types.
  /// @param punctuator `Punctuator` describing a punctuator (i.e.
  /// operator).
  /// @param types Operand types that the punctuator (or operator) should
  /// accept.
  /// @return Pointer to a `Signature` object if there exists a punctuator that
  /// accepts the types provided in the `types` vector. Otherwise, returns
  /// `nullptr`.
  static Signature *AcceptingSignature(Punctuator punctuator,
                                       std::vector<Type *> &types);

 private:
  static std::unordered_map<Punctuator, std::vector<Signature> > signature_map;
};

#endif  // SIGNATURES_H_
