#ifndef TYPE_H_
#define TYPE_H_

#include <string>

class Type {
public:
    virtual std::string GetTypeString() const = 0;
};

#endif // TYPE_H_
