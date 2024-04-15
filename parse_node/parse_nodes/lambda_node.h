#ifndef LAMBDA_NODE_H_
#define LAMBDA_NODE_H_

#include <parse_node/parse_node.h>

#include "code_block_node.h"
#include "lambda_parameter_node.h"
#include "type_node.h"

class LambdaNode : public ParseNode {
public:
    LambdaNode() : ParseNode(ParseNodeType::LAMBDA_NODE, nullptr) {}

    /// @brief Attach the specified `ParseNode` to `this` node to represent a parameter.
    /// @param parameter_node A `ParseNode` representing a lambda parameter.
    /// @warning This method attaches the its argument using `ParseNode::AppendChild`, appending it
    /// to the end of the children vector field.
    void AddParameterNode(std::unique_ptr<ParseNode> parameter_node);

    /// @brief Attach the specified `ParseNode` to `this` node to represent a return type.
    /// @param parameter_node A `ParseNode` representing a lambda return type.
    /// @warning This method attaches the its argument using `ParseNode::AppendChild`, appending it
    /// to the end of the children vector field.
    void AddReturnTypeNode(std::unique_ptr<ParseNode> return_type_node);

    /// @brief Attach the specified `ParseNode` to `this` node to represent a lambda body.
    /// @param parameter_node A `ParseNode` representing a lambda body.
    /// @warning This method attaches the its argument using `ParseNode::AppendChild`, appending it
    /// to the end of the children vector field.
    void AddLambdaBodyNode(std::unique_ptr<ParseNode> lambda_body);

    std::vector<LambdaParameterNode *> GetParameterNodes() { return parameter_nodes; }
    TypeNode *GetReturnTypeNode() { return return_type_node; }
    ParseNode *GetLambdaBodyNode() { return lambda_body; }

    bool IsAnonymous() const {
        return GetParent()->GetParseNodeType() != ParseNodeType::FUNCTION_DEFINITION_NODE;
    }

    virtual std::string ToString() const override { return "LAMBDA NODE"; }

    virtual void Accept(ParseNodeVisitor& visitor) override;

    virtual llvm::Value *GenerateCode(ParseNodeIRVisitor& visitor) override;

private:
    std::vector<LambdaParameterNode *> parameter_nodes;
    TypeNode *return_type_node;
    ParseNode *lambda_body;
};

#endif // LAMBDA_NODE_H_
