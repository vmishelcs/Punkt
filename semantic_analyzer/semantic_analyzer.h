#ifndef SEMANTIC_ANALYZER_H_
#define SEMANTIC_ANALYZER_H_

#include <parse_node/parse_node.h>

class SemanticAnalyzer {
 public:
  static std::unique_ptr<ParseNode> Analyze(std::unique_ptr<ParseNode> ast);

 private:
  SemanticAnalyzer(std::unique_ptr<ParseNode> ast);

  std::unique_ptr<ParseNode> BeginSemanticAnalysis();

  std::unique_ptr<ParseNode> ast;
};

#endif  // SEMANTIC_ANALYZER_H_
