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
    for (int i = LogType::LOG_TYPE_BEGIN; i < LogType::LOG_TYPE_END; ++i) {
        LogType type = static_cast<LogType>(i);
        this->loggers[type] = std::make_unique<PunktLogger::Logger>(type);
    }
}

void PunktLogger::Log(LogType log_type, std::string message) {
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
    PrintMessage(msg_index);
}

void PunktLogger::Logger::PrintMessage(int msg_index) {
    LOG(ERROR) << '(' << LoggerTypeToString() << ')' << ' ' << messages[msg_index];
}
