#ifndef SIGNATURES_H_
#define SIGNATURES_H_

#include <unordered_map>
#include <optional>
#include <functional>

#include <scanner/punctuator.h>

#include "signature.h"

class Signatures {
public:
    // Returns a pointer to a const Signature object if the specified punctuator (i.e. operator)
    // accepts the types compiled in the Type * vector. If no such signature exists, a nullptr is 
    // returned.
    static Signature const *AcceptingSignature(PunctuatorEnum punctuator,
            std::vector<Type *>& types);

private:
    static std::unordered_map<PunctuatorEnum, std::vector<Signature>> signature_map;
};

#endif // SIGNATURES_H_
