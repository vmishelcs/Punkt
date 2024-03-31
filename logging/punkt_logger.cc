#include <glog/logging.h>

#include "punkt_logger.h"

std::unique_ptr<PunktLogger> PunktLogger::instance = nullptr;

static void LogPrefix(std::ostream &s, const google::LogMessage &m, void *) {
    s << "\u001b[31m" << google::GetLogSeverityName(m.severity()) << "\u001b[0m" << ':';
}

void PunktLogger::Log(LogType log_type, std::string message) {
    PunktLogger *punkt_logger = GetInstance();
    if (!punkt_logger->loggers.contains(log_type)) {
        LogFatalInternalError("unimplemented log type");
    }
    punkt_logger->loggers[log_type]->LogMessage(message);
}

void PunktLogger::LogFatal(std::string message) {
    // Make sure logger is initialized
    GetInstance();
    LOG(ERROR) << message << "\ncompilation terminated.";
    std::exit(1);
}

void *PunktLogger::LogFatalInternalError(std::string message) {
    // Make sure logger is initialized
    GetInstance();
    LOG(FATAL) << "internal error: " << message;
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

void PunktLogger::DumpCompileErrors(FILE *stream) {
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
            LOG(FATAL) << "unknown log type in PunktLogger::Logger::TypeToString";
            return nullptr;
    }
}

void PunktLogger::Logger::LogMessage(std::string message) {
    messages.push_back(message);
}

void PunktLogger::Logger::PrintLogMessage(const std::string &message) {
    LOG(ERROR) << "\u001b[33m" << ToString() << "\u001b[0m" << ": " << message << std::endl;
}

PunktLogger::PunktLogger() {
    google::InitGoogleLogging("punkt");
    google::InstallPrefixFormatter(LogPrefix);
    loggers[LogType::SCANNER] = std::make_unique<PunktLogger::Logger>(LogType::SCANNER);
    loggers[LogType::PARSER] = std::make_unique<PunktLogger::Logger>(LogType::PARSER);
    loggers[LogType::SYMBOL_TABLE] = std::make_unique<PunktLogger::Logger>(LogType::SYMBOL_TABLE);
    loggers[LogType::SEMANTIC_ANALYZER] = std::make_unique<PunktLogger::Logger>(LogType::SEMANTIC_ANALYZER);
}

PunktLogger *PunktLogger::GetInstance() {
    if (instance == nullptr) {
        instance = std::make_unique<PunktLogger>();
    }
    return instance.get();
}
