#include <logging/punkt_logger.h>
#include <scanner/scanner.h>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void ScanFile(fs::path file_path) {
  Scanner scanner(file_path);
  while (scanner.HasNext()) {
    std::cout << *scanner.Next() << std::endl;
  }
  std::cout << *scanner.Next() << std::endl;  // Print EOF token
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Token printer application requires an input file argument\n";
    return 1;
  }

  fs::path input_file_directory = fs::path(INPUT_FILE_DIRECTORY);
  fs::path file_path = input_file_directory / argv[1];

  ScanFile(file_path);

  if (PunktLogger::ThereAreCompileErrors()) {
    PunktLogger::DumpCompileErrorSummary();
  }
}
