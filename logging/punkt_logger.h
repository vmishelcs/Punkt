#ifndef PUNKT_LOGGER_H_
#define PUNKT_LOGGER_H_

#include <iostream>
#include <iomanip>
#include <memory>
#include <unordered_map>
#include <vector>

enum LogType {
    SCANNER,
    LOG_TYPE_BEGIN = SCANNER,

    LOG_TYPE_END
};

class PunktLogger {
public:
    static PunktLogger& GetInstance() { return instance; }

    PunktLogger(const PunktLogger&) = delete;
    void operator=(const PunktLogger&) = delete;

    void Log(LogType log_type, std::string message);

private:
    class Logger {
        Logger();
        friend std::unique_ptr<Logger> std::make_unique<Logger>();
        std::vector<std::string> messages;
    };

    PunktLogger();
    static PunktLogger instance;

    std::unordered_map<LogType, std::unique_ptr<Logger>> loggers;
};

#endif // PUNKT_LOGGER_H_
