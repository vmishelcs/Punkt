#ifndef FUNCTION_PROTOTYPE_NODE_H_
#define FUNCTION_PROTOTYPE_NODE_H_

#include <vector>

#include <parse_node/parse_node.h>
#include <semantic_analyzer/types/type.h>

class FunctionPrototypeNode : public ParseNode {
public:
    FunctionPrototypeNode();

    std::vector<Type *> GetParameterTypes() const;

    Type *GetReturnType() const { return GetChildren().back()->GetType(); }

    virtual std::string ToString() const override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // FUNCTION_PROTOTYPE_NODE_H_
