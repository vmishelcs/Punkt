#include <string>

#include "xml_generator_visitor.h"

std::unique_ptr<XMLTag> XMLTag::CreateStartTag(std::string name) {
    return std::make_unique<XMLTag>(name, XMLTag::Type::START);
}

std::unique_ptr<XMLTag> XMLTag::CreateEndTag(std::string name) {
    return std::make_unique<XMLTag>(name, XMLTag::Type::END);
}

std::unique_ptr<XMLTag> XMLTag::CreateSelfClosingTag(std::string name) {
    return std::make_unique<XMLTag>(name, XMLTag::Type::EMPTY_ELEMENT);
}

XMLTag::XMLTag(std::string name, XMLTag::Type tag_type) : name(name), tag_type(tag_type) {}

void XMLTag::AddAttribute(std::string name, std::string value) {
    if (tag_type == XMLTag::Type::EMPTY_ELEMENT || tag_type == XMLTag::Type::START) {
        attributes[name] = value;
    }
}

std::string XMLTag::ToString() const {
    std::string result = "<";

    if (tag_type == XMLTag::Type::END) {
        result.push_back('/');
    }

    result += name;

    if (tag_type == XMLTag::Type::END) {
        result.push_back('>');
        return result;
    }

    result.push_back(' ');

    for (const auto& [name, value] : attributes) {
        result += (name + "=\"" + value + "\" ");
    }

    if (tag_type != XMLTag::Type::EMPTY_ELEMENT) {
        result.pop_back();
    }
    else {
        result.push_back('/');
    }

    result.push_back('>');

    return result;
}

XMLGeneratorVisitor::XMLGeneratorVisitor(std::ostream& output_stream)
    : output_stream(output_stream)
{
    output_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" << std::endl;
    this->depth = 0;
}

// Non-leaf nodes
void XMLGeneratorVisitor::VisitEnter(CodeBlockNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("CodeBlockNode");

    AddBasicParseNodeAttributes(*tag, node);

    OutputTag(*tag);
    ++depth;
}
void XMLGeneratorVisitor::VisitLeave(CodeBlockNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("CodeBlockNode");

    --depth;
    OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(DeclarationStatementNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("DeclarationStatementNode");

    AddBasicParseNodeAttributes(*tag, node);

    OutputTag(*tag);
    ++depth;
}
void XMLGeneratorVisitor::VisitLeave(DeclarationStatementNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("DeclarationStatementNode");
    
    --depth;
    OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(MainNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("MainNode");

    AddBasicParseNodeAttributes(*tag, node);

    OutputTag(*tag);
    ++depth;
}
void XMLGeneratorVisitor::VisitLeave(MainNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("MainNode");

    --depth;
    OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(OperatorNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("OperatorNode");

    tag->AddAttribute("operator", node.GetToken().GetLexeme());
    AddBasicParseNodeAttributes(*tag, node);

    OutputTag(*tag);
    ++depth;
}
void XMLGeneratorVisitor::VisitLeave(OperatorNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("OperatorNode");

    --depth;
    OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(PrintStatementNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("PrintStatementNode");

    AddBasicParseNodeAttributes(*tag, node);

    OutputTag(*tag);
    ++depth;
}
void XMLGeneratorVisitor::VisitLeave(PrintStatementNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("PrintStatementNode");

    --depth;
    OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(ProgramNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("ProgramNode");

    AddBasicParseNodeAttributes(*tag, node);

    OutputTag(*tag);
    ++depth;
}
void XMLGeneratorVisitor::VisitLeave(ProgramNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("ProgramNode");

    --depth;
    OutputTag(*tag);
}

// Leaf nodes
void XMLGeneratorVisitor::Visit(ErrorNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("ErrorNode");

    AddBasicParseNodeAttributes(*tag, node);

    OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(IdentifierNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("IdentifierNode");

    tag->AddAttribute("name", node.GetName());
    AddBasicParseNodeAttributes(*tag, node);

    OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(BooleanLiteralNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("BooleanLiteralNode");

    AddBasicParseNodeAttributes(*tag, node);
    tag->AddAttribute("value", std::to_string(node.GetValue()));

    OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(IntegerLiteralNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("IntegerLiteralNode");

    AddBasicParseNodeAttributes(*tag, node);
    tag->AddAttribute("value", std::to_string(node.GetValue()));

    OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(StringLiteralNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("StringLiteralNode");

    AddBasicParseNodeAttributes(*tag, node);
    tag->AddAttribute("value", node.GetValue());

    OutputTag(*tag);
}

void XMLGeneratorVisitor::AddBasicParseNodeAttributes(XMLTag& tag, ParseNode& node) {
    if (node.HasScope()) { tag.AddAttribute("scope", node.GetScope().GetAttributeString()); }
    if (node.HasType()) { tag.AddAttribute("type", node.GetType().GetAttributeString()); }
}

void XMLGeneratorVisitor::OutputTag(XMLTag& tag) {
    for (int i = 0; i < depth; ++i) {
        output_stream << "    ";
    }
    output_stream << tag.ToString() << std::endl;
}
