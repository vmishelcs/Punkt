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

    if (attributes.size() > 0) {
        result.push_back(' ');
    }

    for (const auto& [name, value] : attributes) {
        result += (name + "=\"" + value + "\" ");
    }

    if (attributes.size() > 0) {
        result.pop_back();
    }

    if (tag_type == XMLTag::Type::EMPTY_ELEMENT) {
        result.push_back(' ');
        result.push_back('/');
    }
    result.push_back('>');

    return result;
}

XMLGeneratorVisitor::XMLGeneratorVisitor(std::ostream& output_stream)
    : output_stream(output_stream)
{
    output_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" << std::endl;
}

// Non-leaf nodes
void XMLGeneratorVisitor::VisitEnter(CodeBlockNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("CodeBlockNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::VisitLeave(CodeBlockNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("CodeBlockNode");
    output_stream << tag->ToString() << std::endl;
}

void XMLGeneratorVisitor::VisitEnter(DeclarationStatementNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("DeclarationStatementNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::VisitLeave(DeclarationStatementNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("DeclarationStatementNode");
    output_stream << tag->ToString() << std::endl;
}

void XMLGeneratorVisitor::VisitEnter(MainNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("MainNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::VisitLeave(MainNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("MainNode");
    output_stream << tag->ToString() << std::endl;
}

void XMLGeneratorVisitor::VisitEnter(OperatorNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("OperatorNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::VisitLeave(OperatorNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("OperatorNode");
    output_stream << tag->ToString() << std::endl;
}

void XMLGeneratorVisitor::VisitEnter(PrintStatementNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("PrintStatementNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::VisitLeave(PrintStatementNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("PrintStatementNode");
    output_stream << tag->ToString() << std::endl;
}

void XMLGeneratorVisitor::VisitEnter(ProgramNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("ProgramNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::VisitLeave(ProgramNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("ProgramNode");
    output_stream << tag->ToString() << std::endl;
}

// Leaf nodes
void XMLGeneratorVisitor::Visit(ErrorNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("ErrorNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::Visit(IdentifierNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("IdentifierNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::Visit(IntegerLiteralNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("IntegerLiteralNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}
void XMLGeneratorVisitor::Visit(StringLiteralNode& node) {
    std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("StringLiteralNode");
    AddParseNodeAttributes(*tag, node);
    output_stream << tag->ToString() << std::endl;
}

void XMLGeneratorVisitor::AddParseNodeAttributes(XMLTag& tag, ParseNode& node) {
    tag.AddAttribute("type",
            node.HasType() ? node.GetType().GetAttributeString() : "null");
    tag.AddAttribute("scope",
            node.HasScope() ? node.GetScope().GetAttributeString() : "null");
}
