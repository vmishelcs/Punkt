#include <code_generator/code_generator.h>
#include <llvm/Support/raw_ostream.h>
#include <logging/punkt_logger.h>
#include <parser/parser.h>
#include <semantic_analyzer/semantic_analyzer.h>
#include <utilities/xml_generator_visitor.h>

#include <filesystem>
#include <iostream>
#include <map>

static void MapOptions(int argc, char *argv[],
                       std::map<int, std::string> &option_map);
static bool IsOption(std::string str);
static fs::path GetInputFilePath(const char *input);
static fs::path GetFilePath(const char *file_path_str);
static void Compile(fs::path input_file_path, fs::path output_file_path,
                    bool dump_ast, fs::path dump_ast_file_path,
                    bool dump_decorated_ast,
                    fs::path dump_decorated_ast_file_path);
static void CheckForErrors();

int main(int argc, char *argv[]) {
  if (argc < 2) {
    PunktLogger::LogFatal("no input file");
  }

  fs::path input_file_path = GetInputFilePath(argv[1]);
  fs::path output_file_path = fs::path(input_file_path).replace_extension("ll");
  bool dump_ast = false;
  fs::path dump_ast_file_path;
  bool dump_decorated_ast = false;
  fs::path dump_decorated_ast_file_path;

  std::map<int, std::string> option_map;
  MapOptions(argc, argv, option_map);

  for (const auto &[idx, option] : option_map) {
    if (option == "-o") {
      // Make sure idx + 1 exists and that it is not an option itself
      if (idx + 1 < argc && !option_map.contains(idx + 1)) {
        output_file_path = GetFilePath(argv[idx + 1]);
      } else {
        PunktLogger::LogFatal("missing filename after \'" + option + "\'");
      }
    } else if (option == "--dump-ast") {
      dump_ast = true;
      if (idx + 1 < argc && !option_map.contains(idx + 1)) {
        dump_ast_file_path = GetFilePath(argv[idx + 1]);
      }
    } else if (option == "--dump-decorated-ast") {
      dump_decorated_ast = true;
      if (idx + 1 < argc && !option_map.contains(idx + 1)) {
        dump_decorated_ast_file_path = GetFilePath(argv[idx + 1]);
      }
    }
  }

  std::cout << "input file:  " << input_file_path.string() << std::endl;
  std::cout << "output file: " << output_file_path.string() << std::endl;
  if (dump_ast) {
    std::cout << "dumping AST to "
              << (dump_ast_file_path.empty() ? "stdout"
                                             : dump_ast_file_path.string())
              << std::endl;
  }
  if (dump_decorated_ast) {
    std::cout << "dumping decorated AST to "
              << (dump_decorated_ast_file_path.empty()
                      ? "stdout"
                      : dump_decorated_ast_file_path.string())
              << std::endl;
  }

  Compile(input_file_path, output_file_path, dump_ast, dump_ast_file_path,
          dump_decorated_ast, dump_decorated_ast_file_path);

  return 0;
}

static void MapOptions(int argc, char *argv[],
                       std::map<int, std::string> &option_map) {
  for (int i = 2; i < argc; ++i) {
    if (IsOption(argv[i])) {
      option_map[i] = argv[i];
    }
  }
}

static bool IsOption(std::string str) {
  return str == "-o" || str == "--dump-ast" || str == "--dump-decorated-ast";
}

static fs::path GetInputFilePath(const char *input_file_path_str) {
  fs::path input_file_path = fs::absolute(fs::path(input_file_path_str));
  if (!fs::exists(input_file_path)) {
    PunktLogger::LogFatal("file " + input_file_path.string() +
                          " does not exist");
  }
  return input_file_path;
}

static fs::path GetFilePath(const char *file_path_str) {
  fs::path file_path = fs::absolute(fs::path(file_path_str));

  if (!fs::exists(file_path.parent_path())) {
    PunktLogger::LogFatal("directory " + file_path.parent_path().string() +
                          " does not exist");
  }

  return file_path;
}

static void Compile(fs::path input_file_path, fs::path output_file_path,
                    bool dump_ast, fs::path dump_ast_file_path,
                    bool dump_decorated_ast,
                    fs::path dump_decorated_ast_file_path) {
  auto ast = Parser::Parse(input_file_path);
  CheckForErrors();

  if (dump_ast) {
    // TODO: Implement output to files
    XMLGeneratorVisitor xml_generator_visitor(std::cout);
    ast->Accept(xml_generator_visitor);
  }

  auto decorated_ast = SemanticAnalyzer::Analyze(std::move(ast));
  CheckForErrors();

  if (dump_decorated_ast) {
    // TODO: Implement output to files
    XMLGeneratorVisitor xml_generator_visitor(std::cout);
    decorated_ast->Accept(xml_generator_visitor);
  }

  CodeGenerator::GenerateIR(std::move(decorated_ast), output_file_path);
}

static void CheckForErrors() {
  if (PunktLogger::ThereAreCompileErrors()) {
    PunktLogger::DumpCompileErrorSummary();
    llvm::errs() << "program has errors. no output file generated.\n";
    std::exit(1);
  }
}
