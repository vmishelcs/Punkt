#include <glog/logging.h>

#include "punkt_logger.h"

static void LogPrefix(std::ostream &s, const google::LogMessageInfo &l, void *) {
   s << l.severity;
}

PunktLogger::PunktLogger() {
    ::google::InitGoogleLogging("", LogPrefix);
    loggers[LogType::SCANNER] = std::make_unique<PunktLogger::Logger>(LogType::SCANNER);
    loggers[LogType::PARSER] = std::make_unique<PunktLogger::Logger>(LogType::PARSER);
    loggers[LogType::SYMBOL_TABLE] = std::make_unique<PunktLogger::Logger>(LogType::SYMBOL_TABLE);
    loggers[LogType::SEMANTIC_ANALYZER] = std::make_unique<PunktLogger::Logger>(LogType::SEMANTIC_ANALYZER);
}

void PunktLogger::Log(LogType log_type, std::string message) {
    if (!loggers.contains(log_type)) {
        std::runtime_error("Logger type not stored in logger map");
    }
    loggers.at(log_type)->LogMessage(message);
}

const char *PunktLogger::Logger::AsString() {
    switch (logger_type) {
        case LogType::SCANNER:
            return "SCANNER";
        case LogType::PARSER:
            return "PARSER";
        case LogType::SYMBOL_TABLE:
            return "SYMBOL TABLE";
        case LogType::SEMANTIC_ANALYZER:
            return "SEMANTIC ANALYZER";
        default:
            LOG(FATAL) << "Unknown log type in PunktLogger::Logger::TypeToString";
    }
}

void PunktLogger::Logger::LogMessage(std::string message) {
    int msg_index = messages.size();
    messages.push_back(message);
    PrintStoredMessage(msg_index);
}

void PunktLogger::Logger::PrintStoredMessage(int msg_index) {
    LOG(ERROR) << "- " << AsString() << ": " << messages[msg_index] << std::endl;
}
