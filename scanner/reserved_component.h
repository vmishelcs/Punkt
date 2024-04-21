#ifndef RESERVED_COMPONENT_H_
#define RESERVED_COMPONENT_H_

#include <string>

class ReservedComponent {
 public:
  ReservedComponent(std::string lexeme) : lexeme(lexeme) {}
  ReservedComponent(ReservedComponent &&reserved_component)
      : lexeme(std::move(reserved_component.lexeme)) {}

  std::string GetLexeme() const { return lexeme; }

 private:
  std::string lexeme;
};

#endif  // RESERVED_COMPONENT_H_
