#include "Logger.h"

Logger* Logger::instance = nullptr;

Logger& Logger::getInstance() {
    if (!instance) {
        instance = new Logger();
    }
    return *instance;
}

void Logger::init(const std::string& filename) {
    logFile.open(filename, std::ios::app);
    if (logFile.is_open()) {
    }
}

void Logger::log(const std::string& message) {
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
    delete instance;
}