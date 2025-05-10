#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>    // For std::ofstream (though implementation is in .cpp)
#include <filesystem> // For std::filesystem::path (though implementation is in .cpp)
#include <chrono>     // For timestamps (though implementation is in .cpp)
#include <iomanip>    // For std::put_time (though implementation is in .cpp)

// Forward declaration for console color setting if needed directly in header (usually not)
// #ifdef _WIN32
// #include <windows.h> // For WORD, if you were to pass colors through here
// #endif

// Enum for log levels
enum class LogLevel {
    INFO,
    WARNING,
    ERR, // Renamed from ERROR to avoid conflict with a windows.h macro if windows.h is included here
    DEBUG
};

// --- Global Path Constants (Declared as extern, defined in logger.cpp) ---
// This allows them to be accessible from any file including logger.h,
// but they are defined (initialized) only once in logger.cpp.
// Alternatively, make them static members of a Logger class, or pass paths to logMessage.
// extern const std::string PROJECT_ROOT_FOR_PATHS; // Or manage this differently
// extern const std::string LOG_DIR_PATH;
// extern const std::string LOG_FILE_PATH;


// --- Core Logging Function Declaration ---
// The `alsoPrintToConsole` parameter allows controlling if the log message
// also appears on the standard output/error streams.
void logMessage(LogLevel level, const std::string& message, bool alsoPrintToConsole = false);


// --- Optional: Convenience Macros for Logging (Reduces typing) ---
// These are optional but can make logging calls shorter.
// Note: If using macros, be mindful of multiple evaluations if arguments have side effects.
// For simple string literals or variables, it's fine.

#define LOG_INFO(message) logMessage(LogLevel::INFO, (message))
#define LOG_INFO_CONSOLE(message) logMessage(LogLevel::INFO, (message), true)

#define LOG_WARNING(message) logMessage(LogLevel::WARNING, (message))
#define LOG_WARNING_CONSOLE(message) logMessage(LogLevel::WARNING, (message), true)

#define LOG_ERR(message) logMessage(LogLevel::ERR, (message)) // Using ERR to avoid potential macro conflict
#define LOG_ERR_CONSOLE(message) logMessage(LogLevel::ERR, (message), true)

#define LOG_DEBUG(message) logMessage(LogLevel::DEBUG, (message))
#define LOG_DEBUG_CONSOLE(message) logMessage(LogLevel::DEBUG, (message), true)


// --- Optional: Initialization Function ---
// Could be used to ensure log directory exists at program start,
// or to set global log file path if not using constants directly.
// void initializeLogger(const std::string& rootPath = ""); // Example


#endif // LOGGER_H