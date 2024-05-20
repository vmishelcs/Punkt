#include "punkt_logger.h"

#include <execinfo.h>
#include <input_handler/text_location.h>
#include <llvm/Support/raw_ostream.h>

#include <filesystem>
#include <fstream>
#include <string>

const unsigned kMaxCompileErrors = 8;

std::unique_ptr<PunktLogger> PunktLogger::instance = nullptr;

void PunktLogger::LogCompileError(const TextLocation &error_location,
                                  const std::string &error_message) {
  PunktLogger *punkt_logger = GetInstance();
  if (punkt_logger->compile_errors.size() >= kMaxCompileErrors) {
    ++(punkt_logger->num_suppressed_errors);
    return;
  }

  // Open the input file for reading.
  const std::string &file_name = error_location.file_name;
  std::ifstream input_file(file_name);

  // Find the line right before the error line.
  int line_num = error_location.line;
  input_file.seekg(std::ios::beg);
  for (int i = 1; i <= line_num - 2; ++i) {
    input_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  // If we can, include the line above the error line.
  std::string above_error_line;
  if (line_num - 1 > 0) {
    std::getline(input_file, above_error_line);
  }

  // Read the line that caused the error.
  std::string error_line;
  std::getline(input_file, error_line);

  // If we can, include the line below the error line.
  std::string below_error_line;
  if (input_file.peek() != EOF) {
    std::getline(input_file, below_error_line);
  }

  // Construct complete log message that has format
  // <text location of error>:  error:  <error message>
  // <above error location line>
  // <error location line>
  // <below error location line>
  std::string complete_message = file_name;
  complete_message += ":" + std::to_string(error_location.line);
  complete_message += ":" + std::to_string(error_location.column);
  complete_message += ": error: " + error_message + "\n";
  if (!above_error_line.empty()) {
    complete_message += "    " + above_error_line + "\n";
  }
  complete_message += "--> " + error_line + "\n";
  if (!below_error_line.empty()) {
    complete_message += "    " + below_error_line + "\n";
  }

  punkt_logger->InsertLog(complete_message);
}

void PunktLogger::InsertLog(const std::string &log_message) {
  this->compile_errors.emplace_back(log_message);
}

void PunktLogger::LogFatal(std::string message) {
  std::cerr << "ERROR: " << message << "\ncompilation terminated." << std::endl;
  std::exit(1);
}

void PunktLogger::LogFatalInternalError(std::string message) {
  std::cerr << "interal error: " << message << std::endl;
  std::exit(-1);
}

bool PunktLogger::ThereAreCompileErrors() {
  PunktLogger *punkt_logger = GetInstance();
  return punkt_logger->compile_errors.size() > 0;
}

void PunktLogger::DumpCompileErrorSummary() {
  PunktLogger *punkt_logger = GetInstance();
  punkt_logger->DumpCompileErrors(llvm::errs());
  unsigned total_errors =
      punkt_logger->compile_errors.size() + punkt_logger->num_suppressed_errors;
  llvm::errs() << std::to_string(total_errors) << " error";
  if (total_errors != 1) {
    llvm::errs() << 's';
  }
  llvm::errs() << " generated";

  if (punkt_logger->num_suppressed_errors > 0) {
    llvm::errs() << " (" << std::to_string(punkt_logger->num_suppressed_errors)
                 << " suppressed).\n";
  } else {
    llvm::errs() << ".\n";
  }
}

void PunktLogger::DumpCompileErrors(llvm::raw_ostream &output_stream) {
  for (const auto &msg : compile_errors) {
    output_stream << msg << '\n';
  }
}

PunktLogger *PunktLogger::GetInstance() {
  if (instance == nullptr) {
    instance = std::make_unique<PunktLogger>();
  }
  return instance.get();
}
