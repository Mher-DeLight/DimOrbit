#include "../include/DimEngineZ/logger.h"

namespace DimEngineZ::logger {

std::stringstream outputStream;
int raylibLogLevel = LOG_NONE;

void flush() {
    std::cout << outputStream.rdbuf();
    std::cout << std::flush;
}
void loadToCout() {
    std::cout << outputStream.rdbuf();
}
void showRaylibLogs(int log_level) {
    raylibLogLevel = log_level;
    SetTraceLogLevel(log_level);
}

}; // namespace DimEngineZ::logger