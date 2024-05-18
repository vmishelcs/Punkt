#ifndef PUNKT_LOGGER_H_
#define PUNKT_LOGGER_H_

#include <input_handler/text_location.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <map>
#include <memory>
#include <vector>

enum class LogType { SCANNER, PARSER, SYMBOL_TABLE, SEMANTIC_ANALYZER };

class PunktLogger {
 public:
  friend std::unique_ptr<PunktLogger> std::make_unique<PunktLogger>();

  PunktLogger(const PunktLogger &) = delete;
  void operator=(const PunktLogger &) = delete;

  /// @brief Log a Punkt compilation error for the Punkt programmer.
  /// @param error_location Text location of the error.
  /// @param error_message Error message representing the error.
  static void LogCompileError(const TextLocation &error_location,
                              const std::string &error_message);

  // Logs a fatal error for the Punkt programmer. This should be used for user
  // errors that are unrecoverable (such as absence of input files to the
  // compiler) because this method crashes the compilation process.

  /// @brief Logs a fatal error for the Punkt programmer. This should be used
  /// for user errors that are unrecoverable (such as absence of input files to
  /// the compiler) because this method crashes the compilation process.
  /// @param message Message to be output before crashing.
  [[noreturn]] static void LogFatal(std::string message);

  /// @brief For logging internal errors within the compiler that are not due to
  /// user error. This should only be used in unrecoverable situations (e.g.
  /// error during code generation) as this will crash the program.
  [[noreturn]] static void LogFatalInternalError(std::string message);

  /// @brief Check if there are user-facing compilation errors.
  /// @return `true` if there are user-facing compilation errors, `false`
  /// otherwise.
  static bool ThereAreCompileErrors();

  /// @brief Dump compilation error summary.
  static void DumpCompileErrorSummary();

 private:
  /// @brief Insert a log message to store.
  /// @param log_message Log message to be inserted for storage.
  void InsertLog(const std::string &log_message);

  /// @brief Dump stored compile errors into specified stream.
  /// @param output_stream Stream to dump errors into.
  void DumpCompileErrors(llvm::raw_ostream &output_stream);

  PunktLogger() : num_suppressed_errors(0), compile_errors() {}
  static std::unique_ptr<PunktLogger> instance;
  static PunktLogger *GetInstance();

  unsigned num_suppressed_errors;
  std::vector<std::string> compile_errors;
};

#endif  // PUNKT_LOGGER_H_
