#pragma once
#include "common.h"
#include <iostream>
#include <sstream>

namespace DimEngineZ::logger {

extern std::stringstream outputStream;
extern int raylibLogLevel;

void flush();
void loadToCout();
void showRaylibLogs(int logLevel = LOG_INFO);

template <typename T> void log(const T& message, bool newline = true) {
    outputStream << message;
    if (newline)
        outputStream << "\n";
}

template <typename T>
    requires std::convertible_to<T, std::string>
void flushLog(const T& message, bool newline = true) {
    log(std::string(message), newline);
    flush();
}

template <typename T>
    requires(!std::convertible_to<T, std::string>)
void flushLog(const T& message, bool newline = true) {
    log(message, newline);
    flush();
}
}; // namespace DimEngineZ::logger