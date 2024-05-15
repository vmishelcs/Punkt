#ifndef PUNKT_LOGGER_H_
#define PUNKT_LOGGER_H_

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

  // Logs an error for the Punkt programmer. The 'log_type' parameter is used
  // to display which compilation step encountered the error (e.g. SCANNER,
  // PARSER, SYMBOL_TABLE, etc).
  static void Log(LogType log_type, std::string message);

  // Logs a fatal error for the Punkt programmer. This should be used to user
  // errors that are unrecoverable (such as absence of input files to the
  // compiler) because this method crashes the program.
  static void LogFatal(std::string message);

  /// @brief For logging internal errors within the compiler. This should only
  /// be used in unrecoverable situations (e.g. error during code generation)
  /// as this will crash the program.
  [[noreturn]] static void LogFatalInternalError(std::string message);

  // Returns true if there are user-facing compilation errors.
  static bool ThereAreCompileErrors();

  // Dumps compile errors from each compilation stage into std::cerr stream.
  static void DumpCompileErrors();

 private:
  class Logger {
    friend PunktLogger;
    friend std::unique_ptr<Logger> std::make_unique<Logger>(LogType &&);

    Logger(LogType type) : logger_type(type) {}

    const char *ToString();
    void LogMessage(std::string message);
    void PrintLogMessage(const std::string &message);

    LogType logger_type;
    std::vector<std::string> messages;
  };

  PunktLogger();
  static std::unique_ptr<PunktLogger> instance;
  static PunktLogger *GetInstance();

  std::map<LogType, std::unique_ptr<Logger> > loggers;
};

#endif  // PUNKT_LOGGER_H_
