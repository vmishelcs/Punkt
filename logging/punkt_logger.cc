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

  // Locate and store the error line in the input file.
  const std::string &file_name = error_location.file_name;
  std::ifstream input_file(file_name);
  std::string error_location_line;
  int line_num = error_location.line;
  input_file.seekg(std::ios::beg);
  for (int i = 1; i <= line_num - 1; ++i) {
    input_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  std::getline(input_file, error_location_line);

  // Construct complete log message that has format
  // <text location of error>:  error:  <error message>
  // <error location line>
  std::string complete_message = file_name;
  complete_message += ":" + std::to_string(error_location.line);
  complete_message += ":" + std::to_string(error_location.column);
  complete_message += "  error:  " + error_message;
  complete_message += "\n-->" + error_location_line;

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
  llvm::raw_fd_ostream output_stream(STDERR_FILENO, /*shouldClose=*/true);
  punkt_logger->DumpCompileErrors(output_stream);
  unsigned total_errors =
      punkt_logger->compile_errors.size() + punkt_logger->num_suppressed_errors;
  output_stream << std::to_string(total_errors) << " error";
  if (total_errors != 1) {
    output_stream << 's';
  }
  output_stream << " generated";

  if (punkt_logger->num_suppressed_errors > 0) {
    output_stream << " (" << std::to_string(punkt_logger->num_suppressed_errors)
                  << " suppressed).\n";
  } else {
    output_stream << ".\n";
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
