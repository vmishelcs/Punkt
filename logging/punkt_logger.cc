#include <glog/logging.h>

#include "punkt_logger.h"

std::unique_ptr<PunktLogger> PunktLogger::instance = nullptr;

static void LogPrefix(std::ostream &s, const google::LogMessage &m, void *) {
    s << google::GetLogSeverityName(m.severity()) << ':';
}

void PunktLogger::Log(LogType log_type, std::string message) {
    PunktLogger *punkt_logger = GetInstance();
    if (!punkt_logger->loggers.contains(log_type)) {
        LogFatalInternalError("unimplemented log type");
    }
    punkt_logger->loggers[log_type]->LogMessage(message);
}

void PunktLogger::LogFatal(std::string message) {
    // Make sure glog is initialized
    GetInstance();
    LOG(ERROR) << message << "\ncompilation terminated.";
    std::exit(1);
}

void *PunktLogger::LogFatalInternalError(std::string message) {
    // Make sure glog is initialized
    GetInstance();
    LOG(FATAL) << "internal error: " << message;
    return nullptr;
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
    int msg_index = messages.size();
    messages.push_back(message);
    PrintStoredMessage(msg_index);
}

void PunktLogger::Logger::PrintStoredMessage(int msg_index) {
    LOG(ERROR) << ToString() << ": " << messages[msg_index] << std::endl;
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
