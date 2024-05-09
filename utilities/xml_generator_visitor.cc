#include "xml_generator_visitor.h"

#include <parse_node/parse_nodes/all_nodes.h>

#include <memory>
#include <ostream>
#include <string>

std::unique_ptr<XMLTag> XMLTag::CreateStartTag(std::string name) {
  return std::make_unique<XMLTag>(name, XMLTag::Type::START);
}

std::unique_ptr<XMLTag> XMLTag::CreateEndTag(std::string name) {
  return std::make_unique<XMLTag>(name, XMLTag::Type::END);
}

std::unique_ptr<XMLTag> XMLTag::CreateSelfClosingTag(std::string name) {
  return std::make_unique<XMLTag>(name, XMLTag::Type::EMPTY_ELEMENT);
}

XMLTag::XMLTag(std::string name, XMLTag::Type tag_type)
    : name(name), tag_type(tag_type) {}

void XMLTag::AddAttribute(std::string name, std::string value) {
  if (tag_type == XMLTag::Type::EMPTY_ELEMENT ||
      tag_type == XMLTag::Type::START) {
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

  for (const auto &[name, value] : attributes) {
    result += (name + "=\"" + value + "\" ");
  }

  if (tag_type != XMLTag::Type::EMPTY_ELEMENT) {
    result.pop_back();
  } else {
    result.push_back('/');
  }

  result.push_back('>');

  return result;
}

XMLGeneratorVisitor::XMLGeneratorVisitor(std::ostream &output_stream)
    : output_stream(output_stream) {
  output_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" << std::endl;
  this->depth = 0;
}

/******************************************************************************
 *                               Non-leaf nodes                               *
 ******************************************************************************/
void XMLGeneratorVisitor::VisitEnter(ArrayTypeNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("ArrayTypeNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(ArrayTypeNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("ArrayTypeNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(CallStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("CallStatementNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(CallStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("CallStatementNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(CodeBlockNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("CodeBlockNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(CodeBlockNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("CodeBlockNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(DeclarationStatementNode &node) {
  std::unique_ptr<XMLTag> tag =
      XMLTag::CreateStartTag("DeclarationStatementNode");

  AddBasicParseNodeAttributes(*tag, node);
  tag->AddAttribute("mutable",
                    node.GetToken()->GetLexeme() == "var" ? "true" : "false");

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(DeclarationStatementNode &node) {
  std::unique_ptr<XMLTag> tag =
      XMLTag::CreateEndTag("DeclarationStatementNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(ExpressionStatementNode &node) {
  std::unique_ptr<XMLTag> tag =
      XMLTag::CreateStartTag("ExpressionStatementNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(ExpressionStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("ExpressionStatementNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(ForStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("ForStatementNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(ForStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("ForStatementNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(FunctionDefinitionNode &node) {
  std::unique_ptr<XMLTag> tag =
      XMLTag::CreateStartTag("FunctionDefinitionNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(FunctionDefinitionNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("FunctionDefinitionNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(IfStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("IfStatementNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(IfStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("IfStatementNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(LambdaInvocationNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("LambdaInvocationNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(LambdaInvocationNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("LambdaInvocationNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(LambdaNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("LambdaNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(LambdaNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("LambdaNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(LambdaParameterNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("LambdaParameterNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(LambdaParameterNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("LambdaParameterNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(LambdaTypeNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("LambdaTypeNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(LambdaTypeNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("LambdaTypeNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(MainNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("MainNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(MainNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("MainNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(OperatorNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("OperatorNode");

  tag->AddAttribute("operator", node.GetToken()->GetLexeme());
  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(OperatorNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("OperatorNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(PrintStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("PrintStatementNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(PrintStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("PrintStatementNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(ProgramNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("ProgramNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(ProgramNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("ProgramNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(ReturnStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("ReturnStatementNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(ReturnStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("ReturnStatementNode");

  --depth;
  OutputTag(*tag);
}

void XMLGeneratorVisitor::VisitEnter(WhileStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateStartTag("WhileStatementNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
  ++depth;
}
void XMLGeneratorVisitor::VisitLeave(WhileStatementNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateEndTag("WhileStatementNode");

  --depth;
  OutputTag(*tag);
}

/******************************************************************************
 *                                 Leaf nodes                                 *
 ******************************************************************************/
void XMLGeneratorVisitor::Visit(ErrorNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("ErrorNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(NopNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("NopNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(IdentifierNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("IdentifierNode");

  tag->AddAttribute("name", node.GetName());
  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(BooleanLiteralNode &node) {
  std::unique_ptr<XMLTag> tag =
      XMLTag::CreateSelfClosingTag("BooleanLiteralNode");

  AddBasicParseNodeAttributes(*tag, node);
  tag->AddAttribute("value", std::to_string(node.GetValue()));

  OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(CharacterLiteralNode &node) {
  std::unique_ptr<XMLTag> tag =
      XMLTag::CreateSelfClosingTag("CharacterLiteralNode");

  AddBasicParseNodeAttributes(*tag, node);
  tag->AddAttribute("value", std::to_string(node.GetValue()));

  OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(IntegerLiteralNode &node) {
  std::unique_ptr<XMLTag> tag =
      XMLTag::CreateSelfClosingTag("IntegerLiteralNode");

  AddBasicParseNodeAttributes(*tag, node);
  tag->AddAttribute("value", std::to_string(node.GetValue()));

  OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(StringLiteralNode &node) {
  std::unique_ptr<XMLTag> tag =
      XMLTag::CreateSelfClosingTag("StringLiteralNode");

  AddBasicParseNodeAttributes(*tag, node);
  tag->AddAttribute("value", node.GetValue());

  OutputTag(*tag);
}
void XMLGeneratorVisitor::Visit(BaseTypeNode &node) {
  std::unique_ptr<XMLTag> tag = XMLTag::CreateSelfClosingTag("BaseTypeNode");

  AddBasicParseNodeAttributes(*tag, node);

  OutputTag(*tag);
}

void XMLGeneratorVisitor::AddBasicParseNodeAttributes(XMLTag &tag,
                                                      ParseNode &node) {
  if (node.HasScope()) {
    tag.AddAttribute("scope", node.GetScope()->GetAttributeString());
  }
  if (node.HasType()) {
    tag.AddAttribute("type", node.GetType()->ToString());
  }
}

void XMLGeneratorVisitor::OutputTag(XMLTag &tag) {
  for (int i = 0; i < depth; ++i) {
    output_stream << "    ";
  }
  output_stream << tag.ToString() << std::endl;
}
