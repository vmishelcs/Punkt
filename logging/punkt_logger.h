#ifndef PUNKT_LOGGER_H_
#define PUNKT_LOGGER_H_

#include <iostream>
#include <iomanip>
#include <memory>
#include <unordered_map>
#include <vector>

// TODO: Change this to an enum class and adjust PunktLogger implementation accordingly.
enum LogType {
    LOG_TYPE_BEGIN,

    SCANNER = LOG_TYPE_BEGIN,
    PARSER,

    LOG_TYPE_END
};

class PunktLogger {
public:
    static PunktLogger& GetInstance() {
        static PunktLogger instance;
        return instance;
    }

    PunktLogger(const PunktLogger&) = delete;
    void operator=(const PunktLogger&) = delete;

    void Log(LogType log_type, std::string message);

private:
    class Logger {
        friend PunktLogger;
        friend std::unique_ptr<Logger> std::make_unique<Logger>(LogType&);

        Logger(LogType type) : logger_type(type) {}
        
        const char *LoggerTypeToString();
        void LogMessage(std::string message);
        void PrintMessage(int msg_index);

        LogType logger_type;
        std::vector<std::string> messages;
    };

    PunktLogger();
    static PunktLogger instance;

    std::unordered_map<LogType, std::unique_ptr<Logger>> loggers;
};

#endif // PUNKT_LOGGER_H_
