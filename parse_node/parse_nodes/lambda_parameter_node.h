#ifndef LAMBDA_PARAMETER_NODE_H_
#define LAMBDA_PARAMETER_NODE_H_

#include <string>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <parse_node/parse_node.h>
#include <token/token.h>

#include "identifier_node.h"
#include "type_node.h"

class LambdaParameterNode : public ParseNode {
public:
    LambdaParameterNode() : ParseNode(ParseNodeType::LAMBDA_PARAMETER_NODE, nullptr) {}

    TypeNode *GetTypeNode() const {
        return dynamic_cast<TypeNode *>(GetChild(0));
    }

    IdentifierNode* GetIdentifierNode() const {
        return dynamic_cast<IdentifierNode *>(GetChild(1));
    }

    static std::unique_ptr<LambdaParameterNode> CreateParameterNode(
            std::unique_ptr<ParseNode> type,
            std::unique_ptr<ParseNode> identifier);

    virtual std::string ToString() const override { return "LAMBDA PARAMETER NODE"; }

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;
};

#endif // LAMBDA_PARAMETER_NODE_H_
