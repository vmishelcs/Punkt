#include <glog/logging.h>

#include "punkt_logger.h"

static void LogPrefix(std::ostream &s, const google::LogMessageInfo &l, void *) {
   s << l.severity << ' '
   << std::setw(2) << l.time.hour() << ':'
   << std::setw(2) << l.time.min()  << ':'
   << std::setw(2) << l.time.sec();
}

PunktLogger::PunktLogger() {
    ::google::InitGoogleLogging("", LogPrefix);
    loggers[LogType::SCANNER] = std::make_unique<PunktLogger::Logger>(LogType::SCANNER);
    loggers[LogType::PARSER] = std::make_unique<PunktLogger::Logger>(LogType::PARSER);
}

void PunktLogger::Log(LogType log_type, std::string message) {
    if (loggers.count(log_type) == 0) {
        std::runtime_error("Logger type not stored in logger map");
    }
    loggers[log_type]->LogMessage(message);
}

const char *PunktLogger::Logger::LoggerTypeToString() {
    std::string result;
    switch (logger_type) {
        case LogType::SCANNER:
            return "SCANNER";
        case LogType::PARSER:
            return "PARSER";
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
    LOG(ERROR) << '(' << LoggerTypeToString() << ')' << ' ' << messages[msg_index];
}
