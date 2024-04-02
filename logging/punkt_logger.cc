#include <execinfo.h>

#include "punkt_logger.h"

std::unique_ptr<PunktLogger> PunktLogger::instance = nullptr;

void PunktLogger::Log(LogType log_type, std::string message) {
    PunktLogger *punkt_logger = GetInstance();
    if (!punkt_logger->loggers.contains(log_type)) {
        LogFatalInternalError("unimplemented log type");
    }
    punkt_logger->loggers[log_type]->LogMessage(message);
}

void PunktLogger::LogFatal(std::string message) {
    std::cerr << "\u001b[31mERROR\u001b[0m: " << message
            << "\ncompilation terminated." << std::endl;
    std::exit(1);
}

void *PunktLogger::LogFatalInternalError(std::string message) {
    std::cerr << "\u001b[31minteral error\u001b[0m: " << message << std::endl;
    std::exit(1);

    return nullptr;
}

bool PunktLogger::ThereAreCompileErrors() {
    PunktLogger *logger = GetInstance();
    for (const auto& entry : logger->loggers) {
        if (!entry.second->messages.empty()) {
            return true;
        }
    }
    return false;
}

void PunktLogger::DumpCompileErrors() {
    PunktLogger *logger = GetInstance();
    for (const auto& entry : logger->loggers) {
        for (const auto& msg : entry.second->messages) {
            entry.second->PrintLogMessage(msg);
        }
    }
}

const char *PunktLogger::Logger::ToString() {
    switch (logger_type) {
        case LogType::SCANNER:
            return "scanner";
        case LogType::PARSER:
            return "parser";
        case LogType::SYMBOL_TABLE:
            return "symbol table";
        case LogType::SEMANTIC_ANALYZER:
            return "semantic analyzer";
        default:
            return (const char *)PunktLogger::LogFatalInternalError(
                    "unimplemented LogType in PunktLogger::Logger::ToString");
    }
}

void PunktLogger::Logger::LogMessage(std::string message) {
    messages.push_back(message);
}

void PunktLogger::Logger::PrintLogMessage(const std::string &message) {
    std::string logger_type_string = ToString();
    std::cerr << "\u001b[31mERROR\u001b[0m: "
            << "\u001b[33m" << logger_type_string
            << "\u001b[0m: " << message << std::endl;
}

PunktLogger::PunktLogger() {
    loggers[LogType::SCANNER] = std::make_unique<PunktLogger::Logger>(LogType::SCANNER);
    loggers[LogType::PARSER] = std::make_unique<PunktLogger::Logger>(LogType::PARSER);
    loggers[LogType::SYMBOL_TABLE] = std::make_unique<PunktLogger::Logger>(LogType::SYMBOL_TABLE);
    loggers[LogType::SEMANTIC_ANALYZER] =
            std::make_unique<PunktLogger::Logger>(LogType::SEMANTIC_ANALYZER);
}

PunktLogger *PunktLogger::GetInstance() {
    if (instance == nullptr) {
        instance = std::make_unique<PunktLogger>();
    }
    return instance.get();
}
