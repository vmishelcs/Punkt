#ifndef XML_GENERATOR_VISITOR_H_
#define XML_GENERATOR_VISITOR_H_

#include <filesystem>
#include <iostream>
#include <map>

#include <parse_node/parse_node_visitor.h>

namespace fs = std::filesystem;

class XMLTag {
private:
    enum class Type {
        START,
        END,
        EMPTY_ELEMENT
    };

public:
    friend std::unique_ptr<XMLTag> std::make_unique<XMLTag>(std::string&, Type&&);

    static std::unique_ptr<XMLTag> CreateStartTag(std::string name);
    static std::unique_ptr<XMLTag> CreateEndTag(std::string name);
    static std::unique_ptr<XMLTag> CreateSelfClosingTag(std::string name);
    
    void AddAttribute(std::string name, std::string value);

    std::string ToString() const;

private:
    XMLTag(std::string name, Type tag_type);

    std::string name;
    Type tag_type;
    std::map<std::string, std::string> attributes;
};

class XMLGeneratorVisitor : public ParseNodeVisitor {
public:
    XMLGeneratorVisitor(std::ostream& output_stream);

    // Non-leaf nodes
    virtual void VisitEnter(CodeBlockNode& node) override;
    virtual void VisitLeave(CodeBlockNode& node) override;

    virtual void VisitEnter(DeclarationStatementNode& node) override;
    virtual void VisitLeave(DeclarationStatementNode& node) override;

    virtual void VisitEnter(IfStatementNode& node) override;
    virtual void VisitLeave(IfStatementNode& node) override;

    virtual void VisitEnter(MainNode& node) override;
    virtual void VisitLeave(MainNode& node) override;

    virtual void VisitEnter(OperatorNode& node) override;
    virtual void VisitLeave(OperatorNode& node) override;

    virtual void VisitEnter(PrintStatementNode& node) override;
    virtual void VisitLeave(PrintStatementNode& node) override;

    virtual void VisitEnter(ProgramNode& node) override;
    virtual void VisitLeave(ProgramNode& node) override;

    // Leaf nodes
    virtual void Visit(ErrorNode& node) override;
    virtual void Visit(IdentifierNode& node) override;
    virtual void Visit(BooleanLiteralNode& node) override;
    virtual void Visit(CharacterLiteralNode& node) override;
    virtual void Visit(IntegerLiteralNode& node) override;
    virtual void Visit(StringLiteralNode& node) override;

private:
    void AddBasicParseNodeAttributes(XMLTag& tag, ParseNode& node);
    void OutputTag(XMLTag& tag);

    std::ostream& output_stream;
};

#endif // XML_GENERATOR_VISITOR_H_
