#ifndef SIGNATURE_H_
#define SIGNATURE_H_

#include <vector>
#include <functional>

#include <semantic_analyzer/type.h>

class Signature {
public:
    Signature(std::initializer_list<Type> input_types, Type output_type);

    const Type& GetOutputType() const;

    bool Accepts(std::vector<std::reference_wrapper<const Type>>& types) const;

private:
    std::vector<Type> input_types;
    Type output_type;
};

#endif // SIGNATURE_H_
