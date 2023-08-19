#include <glog/logging.h>

#include "punkt_logger.h"

static void LogPrefix(std::ostream &s, const google::LogMessageInfo &l, void *) {
   s << l.severity
   << std::setw(4) << 1900 + l.time.year()
   << std::setw(2) << 1 + l.time.month()
   << std::setw(2) << l.time.day()
   << ' '
   << std::setw(2) << l.time.hour() << ':'
   << std::setw(2) << l.time.min()  << ':'
   << std::setw(2) << l.time.sec() << ' '
   << l.filename << ':' << l.line_number << "]";
}

PunktLogger PunktLogger::instance;

PunktLogger::PunktLogger() {
    ::google::InitGoogleLogging("", LogPrefix);
    for (int i = LogType::LOG_TYPE_BEGIN; i < LogType::LOG_TYPE_END; ++i) {
        this->loggers[static_cast<LogType>(i)] = std::make_unique<PunktLogger::Logger>();
    }
}
