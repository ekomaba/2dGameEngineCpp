#include "Logger.h"
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <cstring>

std::vector<LogEntry> Logger::messages;

std::string CurrentDateTimeToString() {
    // TODO: Try to improve this part of the code to use a more modern formatting approach
    // Get the current time from the system clock
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

    // Convert to local time (std::tm)
    std::tm* timeInfo = std::localtime(&nowTime);

    // Format the time into a human-readable string
    std::string output(30, '\0');
    std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", timeInfo);
    
    // Remove any unused characters from the string
    output.resize(std::strlen(output.c_str()));
    
    return output;
}

void Logger::AddLogEntry(LogType type, const std::string& prefix, const std::string& color, const std::string& message) {
    LogEntry logEntry;
    logEntry.type = type;
    logEntry.message = prefix + "[" + CurrentDateTimeToString() + "]: " + message;

    std::cout << color << logEntry.message << "\033[0m" << std::endl;

    messages.push_back(logEntry);
}

void Logger::Log(const std::string &message) {
    AddLogEntry(LOG_INFO, "LOG", "\x1B[32m", message);
}

void Logger::Err(const std::string &message) {
    AddLogEntry(LOG_ERROR, "ERR", "\x1B[91m", message);
}

void Logger::Warn(const std::string& message) {
    AddLogEntry(LOG_WARNING, "WRN", "\x1B[33m", message);
}   