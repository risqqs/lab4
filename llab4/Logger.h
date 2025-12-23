#pragma once
#include <iostream>
#include <fstream>
#include <string>
// Код из DeepSeek'а
class Logger {
private:
    std::ofstream logFile;
    static Logger* instance;

    Logger() = default;

public:
    static Logger& getInstance();
    void init(const std::string& filename = "session.txt");
    void log(const std::string& message);
    ~Logger();
};