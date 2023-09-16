#ifndef IDENTIFIER_NODE_H_
#define IDENTIFIER_NODE_H_

#include <parse_node/parse_node.h>
#include <symbol_table/symbol_table.h>

class IdentifierNode : public ParseNode {
public:
    IdentifierNode(std::unique_ptr<Token> token);

    virtual std::string GetNodeString() override;

    virtual void Accept(ParseNodeVisitor& visitor) override;

    std::optional<std::reference_wrapper<SymbolData>> FindIdentifierSymbolData();
};

#endif // IDENTIFIER_NODE_H_
